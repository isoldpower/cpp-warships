#pragma once

#include <application/head/common/Queries.h>
#include <application/head/common/Theme.h>
#include <application/head/common/state/MenuState.h>
#include <application/head/tui/FtxuiView.h>
#include <application/head/tui/KeyHint.h>
#include <application/head/tui/ScrollPanel.h>

#include <ftxui/dom/elements.hpp>

namespace cpp_warships::head::common {
    class PresentationContext;
}

namespace cpp_warships::head::tui {
    /** @brief Draws the menu: the title, the board sizes on offer and the themes. */
    class MenuView final : public FtxuiRenderer {
    public:
        MenuView(
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
