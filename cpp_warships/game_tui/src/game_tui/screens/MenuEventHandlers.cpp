#include <game_tui/screens/MenuEventHandlers.h>

#include <algorithm>
#include <cstddef>
#include <memory>
#include <utility>

#include <game_tui/ScreenKind.h>
#include <game_tui/Theme.h>
#include <game_tui/intents/ChangeThemeIntent.h>
#include <game_tui/intents/QuitIntent.h>
#include <game_tui/intents/ShowScreenIntent.h>
#include <game_tui/intents/StartMatchIntent.h>

namespace cpp_warships::game_tui {
    namespace {
        constexpr int SMALLEST_BOARD_SIZE = 8;
        constexpr int LARGEST_BOARD_SIZE = 20;
        constexpr int BOARD_SIZE_STEP = 2;

        bool isCharacter(const ftxui::Event& event, const std::string& character) {
            return event.is_character() && event.character() == character;
        }
    } // namespace

    BoardSizeEventHandler::BoardSizeEventHandler(MenuState& state)
        : state_(state) {}

    bool BoardSizeEventHandler::isHandled(ftxui::Event event) const {
        return event == ftxui::Event::ArrowLeft || event == ftxui::Event::ArrowRight;
    }

    void BoardSizeEventHandler::handleEvent(ftxui::Event event) {
        const int step = event == ftxui::Event::ArrowLeft ? -BOARD_SIZE_STEP : BOARD_SIZE_STEP;
        state_.selectedBoardSize = std::clamp(
                state_.selectedBoardSize + step,
                SMALLEST_BOARD_SIZE,
                LARGEST_BOARD_SIZE
        );
    }

    StartMatchEventHandler::StartMatchEventHandler(IntentSink intentSink, const MenuState& state)
        : intentSink_(std::move(intentSink))
        , state_(state) {}

    bool StartMatchEventHandler::isHandled(ftxui::Event event) const {
        return event == ftxui::Event::Return;
    }

    void StartMatchEventHandler::handleEvent(ftxui::Event) {
        intentSink_(std::make_shared<StartMatchIntent>(state_.selectedBoardSize));
    }

    ResumeMatchEventHandler::ResumeMatchEventHandler(
            IntentSink intentSink,
            MatchInProgressQuery hasMatch
    )
        : intentSink_(std::move(intentSink))
        , hasMatch_(std::move(hasMatch)) {}

    bool ResumeMatchEventHandler::isHandled(ftxui::Event event) const {
        return isCharacter(event, "r") && hasMatch_();
    }

    void ResumeMatchEventHandler::handleEvent(ftxui::Event) {
        intentSink_(std::make_shared<ShowScreenIntent>(ScreenKind::Battle));
    }

    CycleThemeEventHandler::CycleThemeEventHandler(IntentSink intentSink, MenuState& state)
        : intentSink_(std::move(intentSink))
        , state_(state) {}

    bool CycleThemeEventHandler::isHandled(ftxui::Event event) const {
        return isCharacter(event, "t");
    }

    void CycleThemeEventHandler::handleEvent(ftxui::Event) {
        const auto& themes = availableThemes();
        state_.selectedThemeIndex =
                (state_.selectedThemeIndex + 1) % static_cast<int>(themes.size());

        const std::size_t index = static_cast<std::size_t>(state_.selectedThemeIndex);
        intentSink_(std::make_shared<ChangeThemeIntent>(themes[index].name));
    }

    QuitEventHandler::QuitEventHandler(IntentSink intentSink)
        : intentSink_(std::move(intentSink)) {}

    bool QuitEventHandler::isHandled(ftxui::Event event) const {
        return isCharacter(event, "q") || event == ftxui::Event::Escape;
    }

    void QuitEventHandler::handleEvent(ftxui::Event) {
        intentSink_(std::make_shared<QuitIntent>());
    }
} // namespace cpp_warships::game_tui
