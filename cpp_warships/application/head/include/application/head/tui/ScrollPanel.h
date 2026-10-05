#pragma once

#include <application/head/common/ScreenKind.h>
#include <application/head/common/input/GridGeometry.h>
#include <application/head/common/input/ScreenRegion.h>
#include <application/head/common/state/PanelState.h>

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/box.hpp>
#include <ftxui/screen/color.hpp>

namespace cpp_warships::head::common {
    class PresentationContext;
}

namespace cpp_warships::head::tui {
    /** @brief How much harder a board's panel gives up height than a small panel does, so
     *  that a short screen squeezes the boards before the lists beside them vanish. */
    inline constexpr int BOARD_PANEL_SHRINK_WEIGHT = 4;

    /** @brief Where a scrolling window landed when it was laid out, and how far its
     * content was scrolled to fit. */
    struct ScrollLayout {
        bool isLaidOut = false;
        ftxui::Box window;
        int contentWidth = 0;
        int contentHeight = 0;
        int offsetX = 0;
        int offsetY = 0;
    };

    /** @brief The colours a scroll bar is painted in. */
    struct ScrollBarColors {
        ftxui::Color track;
        ftxui::Color thumb;
    };

    /** @brief Like FTXUI's reflect, but writes down the whole box an element was laid out in,
     *  even the part a scrolled window hides, so positions inside it stay exact. */
    [[nodiscard]] ftxui::Decorator reflectWholeBox(ftxui::Box& box);

    /** @brief @p content in a window that gives way when room runs short, scrolled as close
     * to @p wanted as fits, with a bar along each edge it overflows. */
    [[nodiscard]] ftxui::Element scrollable(
        ftxui::Element content,
        common::state::ScrollOffset wanted,
        ScrollLayout& layout,
        ScrollBarColors colors
    );

    /** @brief One panel of a screen: a heading that stays put over content that scrolls,
     * marked when it holds the focus. */
    class ScrollPanel {
    public:
        explicit ScrollPanel(common::input::ScreenRegion region) noexcept;

        /** @brief Draws @p content under @p heading, scrolled and focused the way the
         * presentation state of @p screen says. */
        [[nodiscard]] ftxui::Element render(
            const common::PresentationContext& context,
            common::ScreenKind screen,
            ftxui::Element heading,
            ftxui::Element content
        );

        /** @brief Writes down where the panel's window landed when it was last laid out. */
        void publish(common::input::GridGeometry& geometry) const;

    private:
        common::input::ScreenRegion region_;
        ScrollLayout layout_;
    };
}  // namespace cpp_warships::head::tui
