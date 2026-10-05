#include <application/head/common/PresentationContext.h>
#include <application/head/common/input/PanelScrolling.h>
#include <application/head/common/state/BattleState.h>

#include <algorithm>
#include <limits>
#include <map>
#include <utility>

namespace cpp_warships::head::common::input {
    namespace {
        /** @brief How far a panel scrolls when nothing says where its content ends. */
        constexpr int UNBOUNDED_SCROLL = std::numeric_limits<int>::max() / 2;

        /** @brief How far apart two spans are, or nothing when they overlap. */
        [[nodiscard]] int gapBetween(
            const int firstStart,
            const int firstEnd,
            const int secondStart,
            const int secondEnd
        ) {
            return std::max(0, std::max(firstStart, secondStart) - std::min(firstEnd, secondEnd));
        }

        /** @brief How far @p candidate lies from @p from in @p direction, along it and across it,
         * or nothing when it does not lie that way at all. */
        [[nodiscard]] std::optional<std::pair<int, int>> distanceTowards(
            const PanelExtent& from,
            const PanelExtent& candidate,
            const FocusDirection direction
        ) {
            const int fromRight = from.left + from.width - 1;
            const int fromBottom = from.top + from.height - 1;
            const int candidateRight = candidate.left + candidate.width - 1;
            const int candidateBottom = candidate.top + candidate.height - 1;

            const int acrossRows = gapBetween(from.top, fromBottom, candidate.top, candidateBottom);
            const int acrossColumns =
                gapBetween(from.left, fromRight, candidate.left, candidateRight);

            switch (direction) {
                case FocusDirection::Right:
                    if (candidate.left <= fromRight) {
                        return std::nullopt;
                    }
                    return std::pair{acrossRows, candidate.left - fromRight};
                case FocusDirection::Left:
                    if (candidateRight >= from.left) {
                        return std::nullopt;
                    }
                    return std::pair{acrossRows, from.left - candidateRight};
                case FocusDirection::Down:
                    if (candidate.top <= fromBottom) {
                        return std::nullopt;
                    }
                    return std::pair{acrossColumns, candidate.top - fromBottom};
                case FocusDirection::Up:
                    if (candidateBottom >= from.top) {
                        return std::nullopt;
                    }
                    return std::pair{acrossColumns, from.top - candidateBottom};
            }

            return std::nullopt;
        }

        [[nodiscard]] state::ScrollOffset furthestScrollOf(
            const PresentationContext& context,
            const ScreenRegion panel
        ) {
            const std::optional<PanelExtent> extent = context.geometry().panelOf(panel);
            if (extent.has_value()) {
                return {
                    .x = std::max(0, extent->contentWidth - extent->width),
                    .y = std::max(0, extent->contentHeight - extent->height)
                };
            }

            if (panel == ScreenRegion::Log) {
                const auto entries = static_cast<int>(context.game().journal().entries().size());
                return {.x = 0, .y = state::furthestLogScroll(entries)};
            }

            return {.x = UNBOUNDED_SCROLL, .y = UNBOUNDED_SCROLL};
        }

        [[nodiscard]] state::ScrollOffset clampedScroll(
            const state::ScrollOffset wanted,
            const state::ScrollOffset furthest
        ) {
            return {
                .x = std::clamp(wanted.x, 0, furthest.x),
                .y = std::clamp(wanted.y, 0, furthest.y)
            };
        }

        void setScroll(
            state::PresentationState& state,
            const ScreenRegion panel,
            const state::ScrollOffset offset
        ) {
            if (panel == ScreenRegion::Log) {
                state.battle.logScroll = offset.y;
                return;
            }

            state.panels.scrolled[panel] = offset;
        }

        /** @brief The scroll that brings the span from @p first to @p last into a window
         * @p size long that now starts at @p current, moving it as little as it can. */
        [[nodiscard]] int revealedStart(
            const int current,
            const int size,
            const int first,
            const int last
        ) {
            if (size <= 0 || first < current) {
                return first;
            }
            if (last > current + size - 1) {
                return std::min(first, last - size + 1);
            }

            return current;
        }
    }  // namespace

    const std::vector<ScreenRegion>& panelsOf(const ScreenKind screen) {
        static const std::map<ScreenKind, std::vector<ScreenRegion>> PANELS = {
            {ScreenKind::Menu, {ScreenRegion::Settings, ScreenRegion::Shortcuts}},
            {ScreenKind::Saves, {ScreenRegion::SaveList, ScreenRegion::Shortcuts}},
            {ScreenKind::SaveNaming, {ScreenRegion::NameEntry, ScreenRegion::Shortcuts}},
            {ScreenKind::Placement,
             {ScreenRegion::OwnWaters, ScreenRegion::Fleet, ScreenRegion::Shortcuts}},
            {ScreenKind::Battle,
             {ScreenRegion::Log,
              ScreenRegion::EnemyWaters,
              ScreenRegion::OwnWaters,
              ScreenRegion::Skills,
              ScreenRegion::Shortcuts}},
        };

        return PANELS.at(screen);
    }

    ScreenRegion focusedPanel(const state::PresentationState& state, const ScreenKind screen) {
        const std::vector<ScreenRegion>& panels = panelsOf(screen);
        const auto focused = state.panels.focused.find(screen);
        if (focused == state.panels.focused.end() ||
            std::find(panels.begin(), panels.end(), focused->second) == panels.end()) {
            return panels.front();
        }

        return focused->second;
    }

    bool isNewestFirst(const ScreenRegion panel) noexcept {
        return panel == ScreenRegion::Log;
    }

    state::ScrollOffset scrollOf(const state::PresentationState& state, const ScreenRegion panel) {
        if (panel == ScreenRegion::Log) {
            return {.x = 0, .y = state.battle.logScroll};
        }

        const auto scrolled = state.panels.scrolled.find(panel);
        return scrolled == state.panels.scrolled.end() ? state::ScrollOffset{} : scrolled->second;
    }

    void scrollPanel(
        PresentationContext& context,
        const ScreenRegion panel,
        const state::ScrollOffset step
    ) {
        const state::ScrollOffset furthest = furthestScrollOf(context, panel);
        const state::ScrollOffset current =
            clampedScroll(scrollOf(context.state(), panel), furthest);
        const state::ScrollOffset moved{.x = current.x + step.x, .y = current.y + step.y};

        setScroll(context.state(), panel, clampedScroll(moved, furthest));
    }

    void revealInPanel(
        PresentationContext& context,
        const ScreenRegion panel,
        const ScreenArea& wanted
    ) {
        const std::optional<PanelExtent> extent = context.geometry().panelOf(panel);
        if (!extent.has_value()) {
            return;
        }

        const state::ScrollOffset furthest = furthestScrollOf(context, panel);
        const state::ScrollOffset current =
            clampedScroll(scrollOf(context.state(), panel), furthest);
        const state::ScrollOffset revealed{
            .x = revealedStart(current.x, extent->width, wanted.left, wanted.right),
            .y = revealedStart(current.y, extent->height, wanted.top, wanted.bottom)
        };

        setScroll(context.state(), panel, clampedScroll(revealed, furthest));
    }

    void revealCell(
        PresentationContext& context,
        const ScreenRegion board,
        const core::Coordinate cell
    ) {
        const std::optional<PanelExtent> extent = context.geometry().panelOf(board);
        const std::optional<ScreenArea> area = context.geometry().areaOfCell(board, cell);
        if (!extent.has_value() || !area.has_value()) {
            return;
        }

        const int contentOriginX = extent->left - extent->offsetX;
        const int contentOriginY = extent->top - extent->offsetY;
        const ScreenArea wanted{
            .left = cell.x == 0 ? 0 : area->left - contentOriginX,
            .top = cell.y == 0 ? 0 : area->top - contentOriginY,
            .right = area->right - contentOriginX,
            .bottom = area->bottom - contentOriginY
        };

        revealInPanel(context, board, wanted);
    }

    std::optional<ScreenRegion> neighbourOf(
        const GridGeometry& geometry,
        const ScreenKind screen,
        const ScreenRegion from,
        const FocusDirection direction
    ) {
        const auto isShown = [](const PanelExtent& extent) {
            return extent.width > 0 && extent.height > 0;
        };

        const std::optional<PanelExtent> origin = geometry.panelOf(from);
        if (!origin.has_value()) {
            return std::nullopt;
        }

        std::optional<ScreenRegion> nearest;
        std::pair<int, int> nearestDistance;
        for (const ScreenRegion candidate : panelsOf(screen)) {
            const std::optional<PanelExtent> extent = geometry.panelOf(candidate);
            if (candidate == from || !extent.has_value() || !isShown(*extent)) {
                continue;
            }

            const auto distance = distanceTowards(*origin, *extent, direction);
            if (distance.has_value() && (!nearest.has_value() || *distance < nearestDistance)) {
                nearest = candidate;
                nearestDistance = *distance;
            }
        }

        return nearest;
    }
}  // namespace cpp_warships::head::common::input
