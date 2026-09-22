#include <utility>

#include <game_tui/Application.h>
#include <game_tui/intents/ChangeThemeIntent.h>
#include <game_tui/intents/QuitIntent.h>
#include <game_tui/intents/ShowScreenIntent.h>
#include <game_tui/intents/StartMatchIntent.h>

namespace cpp_warships::game_tui {

    ShowScreenIntent::ShowScreenIntent(ScreenKind screen)
        : screen_(screen) {}

    void ShowScreenIntent::applyTo(Application& application) const {
        application.showScreen(screen_);
    }

    StartMatchIntent::StartMatchIntent(int boardSize)
        : boardSize_(boardSize) {}

    void StartMatchIntent::applyTo(Application& application) const {
        application.startNewMatch(boardSize_);
    }

    ChangeThemeIntent::ChangeThemeIntent(std::string themeName)
        : themeName_(std::move(themeName)) {}

    void ChangeThemeIntent::applyTo(Application& application) const {
        application.changeTheme(themeName_);
    }

    void QuitIntent::applyTo(Application& application) const {
        application.requestQuit();
    }
} // namespace cpp_warships::game_tui
