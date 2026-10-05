#include <application/head/common/PresentationContext.h>
#include <application/head/tui/FtxuiPalette.h>
#include <application/head/tui/MenuView.h>
#include <application/head/tui/ScreenParts.h>

#include <string>
#include <vector>

namespace cpp_warships::head::tui {
    namespace {
        /** @brief One setting: what it is, what it is set to, and the keys that change it. */
        ftxui::Element settingLine(
            const common::Theme& theme,
            const std::string& label,
            ftxui::Element value,
            const std::string& keys
        ) {
            return ftxui::hbox(
                {ftxui::text(label) | color(theme.textMuted),
                 std::move(value) | ftxui::bold,
                 ftxui::text("   " + keys) | color(theme.textMuted)}
            );
        }
    }  // namespace

    MenuView::MenuView(
        const common::PresentationContext& context,
        common::input::GridGeometry& geometry
    ) noexcept
        : DialogView(
              context,
              geometry,
              common::ScreenKind::Menu,
              common::input::ScreenRegion::Settings
          ) {}

    ftxui::Element MenuView::title() {
        const common::Theme& theme = context_.theme();
        return ftxui::vbox(
            {ftxui::text("CPP WARSHIPS") | ftxui::bold | color(theme.accent) | ftxui::hcenter,
             ftxui::text("a terminal fleet engagement") | color(theme.textMuted) | ftxui::hcenter,
             divider(theme)}
        );
    }

    std::string MenuView::contentHeading() const {
        return "SETTINGS";
    }

    ftxui::Element MenuView::content() {
        const common::Theme& theme = context_.theme();
        const std::string boardSize = std::to_string(context_.state().menu.selectedBoardSize);

        return ftxui::vbox(
            {settingLine(
                 theme,
                 "board size  ",
                 ftxui::text(boardSize + " x " + boardSize) | color(theme.text),
                 "left right"
             ),
             settingLine(theme, "theme       ", ftxui::text(theme.name) | color(theme.accent), "t")}
        );
    }

    std::vector<KeyHint> MenuView::hints() const {
        const bool hasMatchInProgress = context_.game().hasMatch();
        std::vector<KeyHint> listed{{.key = "enter", .description = "start a new match"}};

        if (hasMatchInProgress) {
            listed.push_back({.key = "r", .description = "resume the match in play"});
            listed.push_back({.key = "s", .description = "name it and quit"});
        }
        if (!hasMatchInProgress && context_.game().saves().hasSavedMatch()) {
            listed.push_back({.key = "l", .description = "load a saved match"});
        }

        listed.push_back({.key = "q", .description = "quit"});
        return listed;
    }
}  // namespace cpp_warships::head::tui
