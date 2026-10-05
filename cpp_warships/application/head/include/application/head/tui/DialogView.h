#pragma once

#include <application/head/common/ScreenKind.h>
#include <application/head/common/input/GridGeometry.h>
#include <application/head/common/input/ScreenRegion.h>
#include <application/head/tui/FtxuiView.h>
#include <application/head/tui/KeyHint.h>
#include <application/head/tui/ScrollPanel.h>

#include <ftxui/dom/elements.hpp>
#include <string>
#include <vector>

namespace cpp_warships::head::common {
    class PresentationContext;
}

namespace cpp_warships::head::tui {
    /** @brief A screen shaped as a dialog: a title, one panel of content, the keys and any
     *  notices, boxed in the middle. Each dialog says only what goes in those parts. */
    class DialogView : public FtxuiRenderer {
    public:
        /** @brief Draws a dialog of @p screen whose content is the panel @p contentRegion. */
        DialogView(
            const common::PresentationContext& context,
            common::input::GridGeometry& geometry,
            common::ScreenKind screen,
            common::input::ScreenRegion contentRegion
        ) noexcept;

    protected:
        [[nodiscard]] ftxui::Element renderElement() final;
        void publishLayout() final;

        /** @brief What stands above the content, if anything. */
        [[nodiscard]] virtual ftxui::Element title();

        /** @brief The heading of the content panel. */
        [[nodiscard]] virtual std::string contentHeading() const = 0;

        /** @brief What the content panel holds. */
        [[nodiscard]] virtual ftxui::Element content() = 0;

        /** @brief The keys this dialog answers to, in the order they are listed. */
        [[nodiscard]] virtual std::vector<KeyHint> hints() const = 0;

        const common::PresentationContext& context_;

    private:
        common::input::GridGeometry& geometry_;
        common::ScreenKind screen_;
        ScrollPanel contentPanel_;
        ScrollPanel shortcutsPanel_;
        KeyHotspots hotspots_;
    };
}  // namespace cpp_warships::head::tui
