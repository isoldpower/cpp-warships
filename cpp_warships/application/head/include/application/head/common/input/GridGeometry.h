#pragma once

#include <application/core/Coordinate.h>
#include <application/head/common/input/Keystroke.h>
#include <application/head/common/input/ScreenRegion.h>

#include <map>
#include <optional>
#include <set>
#include <vector>

namespace cpp_warships::head::common::input {
    /** @brief A rectangle of screen cells, every edge included. */
    struct ScreenArea {
        int left = 0;
        int top = 0;
        int right = 0;
        int bottom = 0;
    };

    /** @brief A spot on screen that presses a key when clicked: a line of the key legend. */
    struct KeyHotspot {
        ScreenArea area;
        Keystroke stroke;
    };

    /** @brief Where a panel's visible window landed, how much content sits behind it, and
     * how far that content was scrolled when it was drawn. */
    struct PanelExtent {
        int left = 0;
        int top = 0;
        int width = 0;
        int height = 0;
        int contentWidth = 0;
        int contentHeight = 0;
        int offsetX = 0;
        int offsetY = 0;

        /** @brief Whether (@p screenX, @p screenY) falls inside the visible window. */
        [[nodiscard]] bool contains(int screenX, int screenY) const;
    };

    /** @brief Where things landed the last time they were drawn, so that a
     * pointer position can be turned back into a cell. */
    class GridGeometry {
    public:
        /** @brief Notes that @p region was drawn as a @p boardWidth by @p boardHeight grid
         * filling the patch from (@p left, @p top) across @p width and down @p height. */
        void rememberBoard(
            ScreenRegion region,
            int left,
            int top,
            int width,
            int height,
            int boardWidth,
            int boardHeight,
            int columnPitch,
            int rowPitch
        );

        /** @brief Notes where the panel @p region was drawn; a board drawn in it is only
         * answerable where this window shows it. */
        void rememberPanel(ScreenRegion region, const PanelExtent& extent);

        /** @brief Notes whether @p panel was drawn where it can fold away. */
        void rememberFoldable(ScreenRegion panel, bool canFold);

        /** @brief Whether @p panel was last drawn where it can fold away. */
        [[nodiscard]] bool isFoldable(ScreenRegion panel) const;

        /** @brief Notes the spots of the key legend that press a key when clicked, in place
         * of whichever were noted before. */
        void rememberHotspots(std::vector<KeyHotspot> hotspots);

        /** @brief The key pressed by clicking (@p screenX, @p screenY), when that is a part of
         * the key legend its panel leaves showing. */
        [[nodiscard]] std::optional<Keystroke> hotspotAt(int screenX, int screenY) const;

        /** @brief Forgets everything, so that a region not drawn this time is
         * not still answering for where it was last time. */
        void clear() noexcept;

        /** @brief Which board, or which panel holding no board, (@p screenX, @p screenY)
         * is over. */
        [[nodiscard]] ScreenRegion regionAt(int screenX, int screenY) const;

        /** @brief Which panel's visible window (@p screenX, @p screenY) is over, boards
         * included. */
        [[nodiscard]] ScreenRegion panelAt(int screenX, int screenY) const;

        /** @brief Where the panel @p region was last drawn, if it was. */
        [[nodiscard]] std::optional<PanelExtent> panelOf(ScreenRegion region) const;

        /** @brief The cell of @p region under (@p screenX, @p screenY), if it
         * is over it. */
        [[nodiscard]] std::optional<core::Coordinate> cellAt(
            ScreenRegion region,
            int screenX,
            int screenY
        ) const;

        /** @brief The screen cells @p cell of the board @p region took up, including the gap
         * after it, wherever scrolling put them. */
        [[nodiscard]] std::optional<ScreenArea> areaOfCell(
            ScreenRegion region,
            core::Coordinate cell
        ) const;

    private:
        /** @brief One drawn patch, and the grid that was drawn into it. */
        struct Patch {
            bool isKnown = false;
            int left = 0;
            int top = 0;
            int width = 0;
            int height = 0;
            int boardWidth = 0;
            int boardHeight = 0;
            int columnPitch = 1;
            int rowPitch = 1;

            [[nodiscard]] bool contains(int screenX, int screenY) const;
            [[nodiscard]] std::optional<core::Coordinate> cellAt(int screenX, int screenY) const;
        };

        /** @brief Whether (@p screenX, @p screenY) is on the part of @p region's grid that
         * its panel left showing. */
        [[nodiscard]] bool isShowing(ScreenRegion region, int screenX, int screenY) const;

        /** @brief Where the board of @p region was drawn, or an unknown patch when it was not. */
        [[nodiscard]] const Patch& patchFor(ScreenRegion region) const;

        std::map<ScreenRegion, Patch> boards_;
        std::map<ScreenRegion, PanelExtent> panels_;
        std::set<ScreenRegion> foldable_;
        std::vector<KeyHotspot> hotspots_;
    };
}  // namespace cpp_warships::head::common::input
