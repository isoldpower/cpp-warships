#pragma once

#include <application/head/tui/FtxuiView.h>
#include <application/head/tui/KeyHint.h>
#include <application/head/tui/ScrollPanel.h>

namespace cpp_warships::head::common {
    class PresentationContext;
}

namespace cpp_warships::head::tui {
    /** @brief The prompt asking what to call the match being put away. */
    class SaveNamingView final : public FtxuiRenderer {
    public:
        SaveNamingView(
            const common::PresentationContext& context,
            common::input::GridGeometry& geometry
        ) noexcept;

    protected:
        [[nodiscard]] ftxui::Element renderElement() override;
        void publishLayout() override;

    private:
        const common::PresentationContext& context_;
        common::input::GridGeometry& geometry_;
        ScrollPanel bodyPanel_;
        ScrollPanel shortcutsPanel_;
        KeyHotspots hotspots_;
    };
}  // namespace cpp_warships::head::tui
