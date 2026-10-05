#pragma once

#include <application/head/common/Theme.h>
#include <application/head/common/input/Keystroke.h>
#include <application/head/common/render/Renderer.h>

#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>

namespace cpp_warships::head::tui {
    /** @brief Below this many columns every screen stacks its panels in one column: about
     *  640 pixels in the browser, where xterm.js draws a column 9 pixels wide by default. */
    inline constexpr int NARROW_LAYOUT_COLUMNS = 71;

    /** @brief Below this many columns, and above the narrow layout, a screen with a side bar
     *  moves it under the boards: about 1017 pixels in the browser, just under 1024. */
    inline constexpr int LAPTOP_LAYOUT_COLUMNS = 113;

    /** @brief What @p event is, said without naming FTXUI, so the game never
     * has to ask. */
    [[nodiscard]] common::input::Keystroke keystrokeOf(ftxui::Event event);

    /** @brief How much room a screen has to lay its panels out in: a phone's single column,
     *  a laptop's boards with a strip under them, or a wide screen with a side bar. */
    enum class LayoutTier {
        Narrow,
        Laptop,
        Wide,
    };

    /** @brief A screen's @p header over its @p body, with @p notices under it, all boxed. */
    [[nodiscard]] ftxui::Element screenFrame(
        const common::Theme& theme,
        ftxui::Element header,
        ftxui::Element body,
        ftxui::Element notices
    );

    /** @brief @p body boxed as a dialog: held at least @p minimumWidth wide in the middle of
     *  the screen, or spread across all of it when the screen @p isNarrow. */
    [[nodiscard]] ftxui::Element dialogFrame(
        const common::Theme& theme,
        ftxui::Element body,
        int minimumWidth,
        bool isNarrow
    );

    /** @brief @p frame as something FTXUI can lay out and draw, cell for cell. */
    [[nodiscard]] ftxui::Element elementOfFrame(const common::render::Frame& frame);

    /** @brief A view that draws with FTXUI, handing back a finished frame like any other. */
    class FtxuiRenderer : public common::render::Renderer {
    public:
        [[nodiscard]] common::render::Frame render(int availableWidth, int availableHeight) final;

    protected:
        /** @brief Draws the view as an element tree, laid out in the room it
         * was offered. */
        [[nodiscard]] virtual ftxui::Element renderElement() = 0;

        /** @brief Writes down where things landed, once the element tree has been laid out. */
        virtual void publishLayout();

        /** @brief Whether the room offered is too narrow for panels to stand side by side. */
        [[nodiscard]] bool isNarrow() const noexcept;

        /** @brief Which layout the room offered calls for. */
        [[nodiscard]] LayoutTier layoutTier() const noexcept;

        /** @brief How many columns the view was offered this time it is drawn. */
        [[nodiscard]] int availableWidth() const noexcept;

    private:
        int availableWidth_ = 0;
    };
}  // namespace cpp_warships::head::tui
