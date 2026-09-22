#pragma once

#include <functional>

#include <ftxui/component/component.hpp>

#include <game_tui/EventRouter.h>
#include <game_tui/Intent.h>
#include <game_tui/Screen.h>
#include <game_tui/Theme.h>
#include <game_tui/screens/MenuState.h>

namespace cpp_warships::game_tui {
    /** @brief Draws the menu: the title, the board sizes on offer and the themes.
     *  Reads state and returns elements; it changes nothing. */
    class MenuView {
    public:
        [[nodiscard]] static ftxui::Element render(
                const Theme& theme,
                const MenuState& state,
                bool hasMatchInProgress
        );
    };

    /** @brief Wires the menu's handlers to its view. It holds no input logic of its own. */
    class MenuController {
    public:
        using MatchInProgressQuery = std::function<bool()>;

        MenuController(IntentSink intentSink, const Theme& theme, MatchInProgressQuery hasMatch);

        [[nodiscard]] ftxui::Component component();

    private:
        const Theme& theme_;
        MatchInProgressQuery hasMatch_;
        MenuState state_;
        EventRouter eventRouter_;
    };

    /** @brief The menu screen, pairing its view with its controller. */
    class MenuScreen final : public Screen {
    public:
        MenuScreen(
                IntentSink intentSink,
                const Theme& theme,
                MenuController::MatchInProgressQuery hasMatch
        );

        [[nodiscard]] ScreenKind kind() const override;
        [[nodiscard]] ftxui::Component component() override;

    private:
        MenuController controller_;
        ftxui::Component component_;
    };
} // namespace cpp_warships::game_tui
