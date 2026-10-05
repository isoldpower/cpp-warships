#pragma once

#include <application/head/tui/DialogView.h>

namespace cpp_warships::head::tui {
    /** @brief The name being typed for the match about to be put away. */
    class SaveNamingView final : public DialogView {
    public:
        SaveNamingView(
            const common::PresentationContext& context,
            common::input::GridGeometry& geometry
        ) noexcept;

    protected:
        [[nodiscard]] std::string contentHeading() const override;
        [[nodiscard]] ftxui::Element content() override;
        [[nodiscard]] std::vector<KeyHint> hints() const override;
    };
}  // namespace cpp_warships::head::tui
