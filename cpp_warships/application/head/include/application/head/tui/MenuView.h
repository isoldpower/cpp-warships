#pragma once

#include <application/head/tui/DialogView.h>

namespace cpp_warships::head::tui {
    /** @brief The menu: the title, the board size and theme being chosen, and what can be done
     * next. */
    class MenuView final : public DialogView {
    public:
        MenuView(
            const common::PresentationContext& context,
            common::input::GridGeometry& geometry
        ) noexcept;

    protected:
        [[nodiscard]] ftxui::Element title() override;
        [[nodiscard]] std::string contentHeading() const override;
        [[nodiscard]] ftxui::Element content() override;
        [[nodiscard]] std::vector<KeyHint> hints() const override;
    };
}  // namespace cpp_warships::head::tui
