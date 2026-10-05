#include <application/head/common/PresentationContext.h>
#include <application/head/tui/DialogView.h>
#include <application/head/tui/FtxuiNotices.h>
#include <application/head/tui/ScreenParts.h>

#include <utility>

namespace cpp_warships::head::tui {
    namespace {
        /** @brief The narrowest a dialog stands on a screen wide enough to centre it. */
        constexpr int MINIMUM_DIALOG_WIDTH = 46;
    }  // namespace

    DialogView::DialogView(
        const common::PresentationContext& context,
        common::input::GridGeometry& geometry,
        const common::ScreenKind screen,
        const common::input::ScreenRegion contentRegion
    ) noexcept
        : context_(context)
        , geometry_(geometry)
        , screen_(screen)
        , contentPanel_(contentRegion)
        , shortcutsPanel_(common::input::ScreenRegion::Shortcuts) {}

    ftxui::Element DialogView::renderElement() {
        const common::Theme& theme = context_.theme();
        const ftxui::Element legend =
            keyLegend(theme, hints(), hotspots_, !context_.state().isKeyboardLayoutFree);

        return dialogFrame(
            theme,
            ftxui::vbox(
                {title(),
                 contentPanel_
                     .render(context_, screen_, sectionHeading(theme, contentHeading()), content()),
                 divider(theme),
                 shortcutsPanel_.render(context_, screen_, sectionHeading(theme, "KEYS"), legend),
                 noticeBlock(theme, context_.application())}
            ),
            MINIMUM_DIALOG_WIDTH,
            isNarrow()
        );
    }

    void DialogView::publishLayout() {
        contentPanel_.publish(geometry_);
        shortcutsPanel_.publish(geometry_);
        hotspots_.publish(geometry_);
    }

    ftxui::Element DialogView::title() {
        return ftxui::emptyElement();
    }
}  // namespace cpp_warships::head::tui
