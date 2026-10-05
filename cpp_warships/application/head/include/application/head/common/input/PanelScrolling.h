#pragma once

#include <application/core/Coordinate.h>
#include <application/head/common/ScreenKind.h>
#include <application/head/common/input/GridGeometry.h>
#include <application/head/common/input/ScreenRegion.h>
#include <application/head/common/state/PanelState.h>
#include <application/head/common/state/PresentationState.h>

#include <optional>
#include <vector>

namespace cpp_warships::head::common {
    class PresentationContext;
}

namespace cpp_warships::head::common::input {
    /** @brief Which way the focus is asked to move from one panel to the next. */
    enum class FocusDirection {
        Up,
        Down,
        Left,
        Right,
    };

    /** @brief How far one roll of the wheel scrolls a panel, and a page when the panel's
     * height is not known. */
    inline constexpr int PANEL_SCROLL_STEP = 3;

    /** @brief The panels @p screen is drawn with, the one that holds the focus at first leading. */
    [[nodiscard]] const std::vector<ScreenRegion>& panelsOf(ScreenKind screen);

    /** @brief The panel holding the focus on @p screen. */
    [[nodiscard]] ScreenRegion focusedPanel(
        const state::PresentationState& state,
        ScreenKind screen
    );

    /** @brief Whether @p panel lists its newest line first, so that scrolling back
     * reads down it. */
    [[nodiscard]] bool isNewestFirst(ScreenRegion panel) noexcept;

    /** @brief How far @p panel is scrolled, as last asked for. */
    [[nodiscard]] state::ScrollOffset scrollOf(
        const state::PresentationState& state,
        ScreenRegion panel
    );

    /** @brief Scrolls @p panel by @p step, stopping at either end of what it holds. */
    void scrollPanel(PresentationContext& context, ScreenRegion panel, state::ScrollOffset step);

    /** @brief Scrolls @p panel as little as it takes for @p wanted, given in the content's
     * own columns and lines, to come into view. */
    void revealInPanel(PresentationContext& context, ScreenRegion panel, const ScreenArea& wanted);

    /** @brief Scrolls the board panel @p board as little as it takes for @p cell to show,
     * along with the labels when the cell sits on the first row or column. */
    void revealCell(PresentationContext& context, ScreenRegion board, core::Coordinate cell);

    /** @brief The panel of @p screen lying nearest @p from in @p direction, if there is one. */
    [[nodiscard]] std::optional<ScreenRegion> neighbourOf(
        const GridGeometry& geometry,
        ScreenKind screen,
        ScreenRegion from,
        FocusDirection direction
    );
}  // namespace cpp_warships::head::common::input
