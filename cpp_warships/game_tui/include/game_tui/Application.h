#pragma once

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>

#include <game_flow/Match.h>
#include <game_flow/RandomEngine.h>
#include <game_tui/Intent.h>
#include <game_tui/Screen.h>
#include <game_tui/ScreenKind.h>
#include <game_tui/Theme.h>

namespace ftxui {
    class App;
} // namespace ftxui

namespace cpp_warships::game_tui {
    /** @brief The running interface: which screen is showing, the theme, the match in play.
     *  Intents act on this, and nothing else does. */
    class Application {
    public:
        explicit Application(game_flow::RandomEngine& randomEngine);
        ~Application();

        /** @brief Shows the interface and returns once the player has quit. */
        void run();

        [[nodiscard]] const Theme& theme() const noexcept;
        [[nodiscard]] ScreenKind currentScreen() const noexcept;
        [[nodiscard]] bool hasMatch() const noexcept;
        [[nodiscard]] game_flow::Match& match();

        void showScreen(ScreenKind screen);
        void startNewMatch(int boardSize);
        void changeTheme(const std::string& themeName);
        void requestQuit();

        /** @brief Hands an intent to the application, the only route a screen has to it. */
        void submit(const IntentPointer& intent);

    private:
        [[nodiscard]] Screen& activeScreen();

        game_flow::RandomEngine& randomEngine_;
        Theme theme_;
        ScreenKind currentScreen_;
        std::optional<game_flow::Match> match_;
        std::map<ScreenKind, std::unique_ptr<Screen>> screens_;
        ftxui::App* interactiveScreen_;
    };
} // namespace cpp_warships::game_tui
