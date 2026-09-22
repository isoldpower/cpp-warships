#include <game_tui/screens/MenuScreen.h>

#include <string>
#include <utility>
#include <vector>

#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>

#include <game_tui/screens/MenuEventHandlers.h>

namespace cpp_warships::game_tui {
    namespace {
        constexpr int MINIMUM_PANEL_WIDTH = 46;

        ftxui::Element titleBlock(const Theme& theme) {
            return ftxui::vbox(
                    {ftxui::text("CPP WARSHIPS") | ftxui::bold | ftxui::color(theme.accent) |
                             ftxui::hcenter,
                     ftxui::text("a terminal fleet engagement") | ftxui::color(theme.textMuted) |
                             ftxui::hcenter}
            );
        }

        ftxui::Element keyHint(
                const Theme& theme,
                const std::string& key,
                const std::string& description
        ) {
            return ftxui::hbox(
                    {ftxui::text(" " + key + " ") | ftxui::color(theme.background) |
                             ftxui::bgcolor(theme.accent),
                     ftxui::text("  " + description) | ftxui::color(theme.text)}
            );
        }
    } // namespace

    ftxui::Element MenuView::render(
            const Theme& theme,
            const MenuState& state,
            bool hasMatchInProgress
    ) {
        std::vector<ftxui::Element> rows{
                titleBlock(theme),
                ftxui::separator() | ftxui::color(theme.border),
                ftxui::hbox(
                        {ftxui::text("board size  ") | ftxui::color(theme.textMuted),
                         ftxui::text(
                                 std::to_string(state.selectedBoardSize) + " x " +
                                 std::to_string(state.selectedBoardSize)
                         ) | ftxui::bold |
                                 ftxui::color(theme.text),
                         ftxui::text("   left right") | ftxui::color(theme.textMuted)}
                ),
                ftxui::hbox(
                        {ftxui::text("theme       ") | ftxui::color(theme.textMuted),
                         ftxui::text(theme.name) | ftxui::bold | ftxui::color(theme.accent),
                         ftxui::text("   t") | ftxui::color(theme.textMuted)}
                ),
                ftxui::separator() | ftxui::color(theme.border),
                keyHint(theme, "enter", "start a new match")
        };

        if (hasMatchInProgress) {
            rows.push_back(keyHint(theme, "r", "resume the match in play"));
        }

        rows.push_back(keyHint(theme, "q", "quit"));

        return ftxui::vbox(std::move(rows)) | ftxui::border |
               ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, MINIMUM_PANEL_WIDTH) |
               ftxui::color(theme.border) | ftxui::bgcolor(theme.background) | ftxui::center;
    }

    MenuController::MenuController(
            IntentSink intentSink,
            const Theme& theme,
            MatchInProgressQuery hasMatch
    )
        : theme_(theme)
        , hasMatch_(std::move(hasMatch)) {
        eventRouter_.add(std::make_shared<BoardSizeEventHandler>(state_));
        eventRouter_.add(std::make_shared<StartMatchEventHandler>(intentSink, state_));
        eventRouter_.add(std::make_shared<ResumeMatchEventHandler>(intentSink, hasMatch_));
        eventRouter_.add(std::make_shared<CycleThemeEventHandler>(intentSink, state_));
        eventRouter_.add(std::make_shared<QuitEventHandler>(intentSink));
    }

    ftxui::Component MenuController::component() {
        const auto renderMenu = [this] {
            return MenuView::render(theme_, state_, hasMatch_());
        };

        const auto dispatchEvent = [this](ftxui::Event event) {
            return eventRouter_.dispatch(event);
        };

        return ftxui::CatchEvent(ftxui::Renderer(renderMenu), dispatchEvent);
    }

    MenuScreen::MenuScreen(
            IntentSink intentSink,
            const Theme& theme,
            MenuController::MatchInProgressQuery hasMatch
    )
        : controller_(std::move(intentSink), theme, std::move(hasMatch))
        , component_(controller_.component()) {}

    ScreenKind MenuScreen::kind() const {
        return ScreenKind::Menu;
    }

    ftxui::Component MenuScreen::component() {
        return component_;
    }
} // namespace cpp_warships::game_tui
