#pragma once

#include <application/head/tui/DialogView.h>

namespace cpp_warships::head::tui {
    /** @brief The saved games as a highlighted list, newest first. */
    class SaveBrowserView final : public DialogView {
    public:
        SaveBrowserView(
            const common::PresentationContext& context,
            common::input::GridGeometry& geometry
        ) noexcept;

    protected:
        [[nodiscard]] std::string contentHeading() const override;
        [[nodiscard]] ftxui::Element content() override;
        [[nodiscard]] std::vector<KeyHint> hints() const override;
    };
}  // namespace cpp_warships::head::tui
