#include <application/head/common/input/GridGeometry.h>

#include <utility>

namespace cpp_warships::head::common::input {
    namespace {
        [[nodiscard]] bool isBoard(const ScreenRegion region) {
            return region == ScreenRegion::OwnWaters || region == ScreenRegion::EnemyWaters;
        }
    }  // namespace

    bool PanelExtent::contains(const int screenX, const int screenY) const {
        return screenX >= left && screenX < left + width && screenY >= top &&
               screenY < top + height;
    }

    bool GridGeometry::Patch::contains(const int screenX, const int screenY) const {
        return isKnown && screenX >= left && screenX < left + width && screenY >= top &&
               screenY < top + height;
    }

    std::optional<core::Coordinate> GridGeometry::Patch::cellAt(
        const int screenX,
        const int screenY
    ) const {
        if (!contains(screenX, screenY) || columnPitch <= 0 || rowPitch <= 0) {
            return std::nullopt;
        }

        const int column = (screenX - left) / columnPitch;
        const int row = (screenY - top) / rowPitch;
        if (column < 0 || column >= boardWidth || row < 0 || row >= boardHeight) {
            return std::nullopt;
        }

        return core::Coordinate{column, row};
    }

    void GridGeometry::rememberBoard(
        const ScreenRegion region,
        const int left,
        const int top,
        const int width,
        const int height,
        const int boardWidth,
        const int boardHeight,
        const int columnPitch,
        const int rowPitch
    ) {
        patchFor(region) = Patch{
            .isKnown = true,
            .left = left,
            .top = top,
            .width = width,
            .height = height,
            .boardWidth = boardWidth,
            .boardHeight = boardHeight,
            .columnPitch = columnPitch,
            .rowPitch = rowPitch
        };
    }

    void GridGeometry::rememberPanel(const ScreenRegion region, const PanelExtent& extent) {
        if (region == ScreenRegion::Elsewhere) {
            return;
        }

        panels_[region] = extent;
    }

    void GridGeometry::rememberFoldable(const ScreenRegion panel, const bool canFold) {
        if (canFold) {
            foldable_.insert(panel);
            return;
        }

        foldable_.erase(panel);
    }

    bool GridGeometry::isFoldable(const ScreenRegion panel) const {
        return foldable_.contains(panel);
    }

    void GridGeometry::rememberHotspots(std::vector<KeyHotspot> hotspots) {
        hotspots_ = std::move(hotspots);
    }

    std::optional<Keystroke> GridGeometry::hotspotAt(const int screenX, const int screenY) const {
        const std::optional<PanelExtent> legend = panelOf(ScreenRegion::Shortcuts);
        if (!legend.has_value() || !legend->contains(screenX, screenY)) {
            return std::nullopt;
        }

        for (const KeyHotspot& hotspot : hotspots_) {
            const ScreenArea& area = hotspot.area;
            if (screenX >= area.left && screenX <= area.right && screenY >= area.top &&
                screenY <= area.bottom) {
                return hotspot.stroke;
            }
        }

        return std::nullopt;
    }

    void GridGeometry::clear() noexcept {
        ownWaters_ = Patch{};
        enemyWaters_ = Patch{};
        panels_.clear();
        foldable_.clear();
        hotspots_.clear();
    }

    ScreenRegion GridGeometry::regionAt(const int screenX, const int screenY) const {
        if (isShowing(ScreenRegion::EnemyWaters, screenX, screenY)) {
            return ScreenRegion::EnemyWaters;
        }
        if (isShowing(ScreenRegion::OwnWaters, screenX, screenY)) {
            return ScreenRegion::OwnWaters;
        }

        const ScreenRegion panel = panelAt(screenX, screenY);
        return isBoard(panel) ? ScreenRegion::Elsewhere : panel;
    }

    ScreenRegion GridGeometry::panelAt(const int screenX, const int screenY) const {
        for (const auto& [region, extent] : panels_) {
            if (extent.contains(screenX, screenY)) {
                return region;
            }
        }

        return ScreenRegion::Elsewhere;
    }

    std::optional<PanelExtent> GridGeometry::panelOf(const ScreenRegion region) const {
        const auto found = panels_.find(region);
        if (found == panels_.end()) {
            return std::nullopt;
        }

        return found->second;
    }

    std::optional<core::Coordinate> GridGeometry::cellAt(
        const ScreenRegion region,
        const int screenX,
        const int screenY
    ) const {
        if (!isShowing(region, screenX, screenY)) {
            return std::nullopt;
        }

        return patchFor(region).cellAt(screenX, screenY);
    }

    std::optional<ScreenArea> GridGeometry::areaOfCell(
        const ScreenRegion region,
        const core::Coordinate cell
    ) const {
        const Patch& patch = patchFor(region);
        if (!patch.isKnown || cell.x < 0 || cell.x >= patch.boardWidth || cell.y < 0 ||
            cell.y >= patch.boardHeight) {
            return std::nullopt;
        }

        const int left = patch.left + cell.x * patch.columnPitch;
        const int top = patch.top + cell.y * patch.rowPitch;
        return ScreenArea{
            .left = left,
            .top = top,
            .right = left + patch.columnPitch - 1,
            .bottom = top + patch.rowPitch - 1
        };
    }

    bool GridGeometry::isShowing(
        const ScreenRegion region,
        const int screenX,
        const int screenY
    ) const {
        if (!patchFor(region).contains(screenX, screenY)) {
            return false;
        }

        const auto panel = panels_.find(region);
        return panel == panels_.end() || panel->second.contains(screenX, screenY);
    }

    const GridGeometry::Patch& GridGeometry::patchFor(const ScreenRegion region) const {
        switch (region) {
            case ScreenRegion::OwnWaters:
                return ownWaters_;
            case ScreenRegion::EnemyWaters:
                return enemyWaters_;
            default:
                break;
        }

        return elsewhere_;
    }

    GridGeometry::Patch& GridGeometry::patchFor(const ScreenRegion region) {
        const GridGeometry& self = *this;
        return const_cast<Patch&>(self.patchFor(region));
    }
}  // namespace cpp_warships::head::common::input
