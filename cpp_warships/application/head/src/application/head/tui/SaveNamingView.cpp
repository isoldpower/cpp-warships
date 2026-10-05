#include <application/head/common/PresentationContext.h>
#include <application/head/tui/FtxuiNotices.h>
#include <application/head/tui/FtxuiPalette.h>
#include <application/head/tui/KeyHint.h>
#include <application/head/tui/SaveNamingView.h>

#include <string>
#include <utility>
#include <vector>

namespace cpp_warships::head::tui {
    namespace {
        constexpr int MINIMUM_PANEL_WIDTH = 46;
        ftxui::Element divider(const common::Theme& theme) {
            return ftxui::separator() | color(theme.border);
        }

        ftxui::Element sectionHeading(const common::Theme& theme, const std::string& title) {
            return ftxui::text(title) | ftxui::bold | color(theme.accent);
        }
    }  // namespace

    SaveNamingView::SaveNamingView(
        const common::PresentationContext& context,
        common::input::GridGeometry& geometry
    ) noexcept
        : context_(context)
        , geometry_(geometry)
        , bodyPanel_(common::input::ScreenRegion::NameEntry)
        , shortcutsPanel_(common::input::ScreenRegion::Shortcuts) {}

    void SaveNamingView::publishLayout() {
        bodyPanel_.publish(geometry_);
        shortcutsPanel_.publish(geometry_);
        hotspots_.publish(geometry_);
    }

    ftxui::Element SaveNamingView::renderElement() {
        const common::Theme& theme = context_.theme();
        const std::string typed = context_.state().naming.typedName;
        constexpr common::ScreenKind SCREEN = common::ScreenKind::SaveNaming;

        std::vector<ftxui::Element> rows{ftxui::hbox(
            {ftxui::text("name  ") | color(theme.textMuted),
             ftxui::text(typed + "_") | ftxui::bold | color(theme.text)}
        )};

        if (typed.empty()) {
            rows.push_back(
                ftxui::text("a name is needed before it can be put away") | color(theme.textMuted)
            );
        }

        std::vector<KeyHint> hints{
            KeyHint{.key = "letters", .description = "type a name"},
            KeyHint{.key = "back", .description = "rub one out"}
        };
        if (!typed.empty()) {
            hints.push_back(KeyHint{.key = "enter", .description = "save and quit"});
        }
        hints.push_back(KeyHint{.key = "esc", .description = "back to the menu"});

        return dialogFrame(
            theme,
            ftxui::vbox(
                {bodyPanel_.render(
                     context_,
                     SCREEN,
                     sectionHeading(theme, "NAME THIS MATCH"),
                     ftxui::vbox(std::move(rows))
                 ),
                 divider(theme),
                 shortcutsPanel_.render(
                     context_,
                     SCREEN,
                     sectionHeading(theme, "KEYS"),
                     keyLegend(theme, std::move(hints), hotspots_)
                 ),
                 noticeBlock(theme, context_.application())}
            ),
            MINIMUM_PANEL_WIDTH,
            isNarrow()
        );
    }
}  // namespace cpp_warships::head::tui
