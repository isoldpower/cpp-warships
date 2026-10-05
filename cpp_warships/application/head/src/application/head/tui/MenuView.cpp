#include <application/head/common/PresentationContext.h>
#include <application/head/tui/FtxuiNotices.h>
#include <application/head/tui/FtxuiPalette.h>
#include <application/head/tui/KeyHint.h>
#include <application/head/tui/MenuView.h>

#include <ftxui/component/event.hpp>
#include <string>
#include <utility>
#include <vector>

namespace cpp_warships::head::tui {
    namespace {
        constexpr int MINIMUM_PANEL_WIDTH = 46;

        ftxui::Element titleBlock(const common::Theme& theme) {
            return ftxui::vbox(
                {ftxui::text("CPP WARSHIPS") | ftxui::bold | color(theme.accent) | ftxui::hcenter,
                 ftxui::text("a terminal fleet engagement") | color(theme.textMuted) |
                     ftxui::hcenter}
            );
        }
        ftxui::Element divider(const common::Theme& theme) {
            return ftxui::separator() | color(theme.border);
        }

        ftxui::Element sectionHeading(const common::Theme& theme, const std::string& title) {
            return ftxui::text(title) | ftxui::bold | color(theme.accent);
        }
    }  // namespace

    MenuView::MenuView(
        const common::PresentationContext& context,
        common::input::GridGeometry& geometry
    ) noexcept
        : context_(context)
        , geometry_(geometry)
        , bodyPanel_(common::input::ScreenRegion::Settings)
        , shortcutsPanel_(common::input::ScreenRegion::Shortcuts) {}

    void MenuView::publishLayout() {
        bodyPanel_.publish(geometry_);
        shortcutsPanel_.publish(geometry_);
        hotspots_.publish(geometry_);
    }

    ftxui::Element MenuView::renderElement() {
        const common::Theme& theme = context_.theme();
        const common::state::MenuState& state = context_.state().menu;
        const bool hasMatchInProgress = context_.game().hasMatch();
        constexpr common::ScreenKind SCREEN = common::ScreenKind::Menu;

        ftxui::Element settings = ftxui::vbox(
            {ftxui::hbox(
                 {ftxui::text("board size  ") | color(theme.textMuted),
                  ftxui::text(
                      std::to_string(state.selectedBoardSize) + " x " +
                      std::to_string(state.selectedBoardSize)
                  ) | ftxui::bold |
                      color(theme.text),
                  ftxui::text("   left right") | color(theme.textMuted)}
             ),
             ftxui::hbox(
                 {ftxui::text("theme       ") | color(theme.textMuted),
                  ftxui::text(theme.name) | ftxui::bold | color(theme.accent),
                  ftxui::text("   t") | color(theme.textMuted)}
             )}
        );

        std::vector<KeyHint> hints{KeyHint{.key = "enter", .description = "start a new match"}};

        if (hasMatchInProgress) {
            hints.push_back(KeyHint{.key = "r", .description = "resume the match in play"});
            hints.push_back(KeyHint{.key = "s", .description = "name it and quit"});
        }

        if (!hasMatchInProgress && context_.game().saves().hasSavedMatch()) {
            hints.push_back(KeyHint{.key = "l", .description = "load a saved match"});
        }

        hints.push_back(KeyHint{.key = "q", .description = "quit"});

        return dialogFrame(
            theme,
            ftxui::vbox(
                {titleBlock(theme),
                 divider(theme),
                 bodyPanel_.render(
                     context_,
                     SCREEN,
                     sectionHeading(theme, "SETTINGS"),
                     std::move(settings)
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
