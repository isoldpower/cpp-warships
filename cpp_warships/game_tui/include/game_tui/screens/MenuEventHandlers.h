#pragma once

#include <functional>

#include <game_tui/EventHandler.h>
#include <game_tui/Intent.h>
#include <game_tui/screens/MenuState.h>

namespace cpp_warships::game_tui {
    /** @brief Widens and narrows the board with the left and right arrows. */
    class BoardSizeEventHandler final : public EventHandler {
    public:
        explicit BoardSizeEventHandler(MenuState& state);

        [[nodiscard]] bool isHandled(ftxui::Event event) const override;
        void handleEvent(ftxui::Event event) override;

    private:
        MenuState& state_;
    };

    /** @brief Starts a match on the chosen board when enter is pressed. */
    class StartMatchEventHandler final : public EventHandler {
    public:
        StartMatchEventHandler(IntentSink intentSink, const MenuState& state);

        [[nodiscard]] bool isHandled(ftxui::Event event) const override;
        void handleEvent(ftxui::Event event) override;

    private:
        IntentSink intentSink_;
        const MenuState& state_;
    };

    /** @brief Returns to a match already in play, and only offers itself when there is one. */
    class ResumeMatchEventHandler final : public EventHandler {
    public:
        using MatchInProgressQuery = std::function<bool()>;

        ResumeMatchEventHandler(IntentSink intentSink, MatchInProgressQuery hasMatch);

        [[nodiscard]] bool isHandled(ftxui::Event event) const override;
        void handleEvent(ftxui::Event event) override;

    private:
        IntentSink intentSink_;
        MatchInProgressQuery hasMatch_;
    };

    /** @brief Steps through the available themes. */
    class CycleThemeEventHandler final : public EventHandler {
    public:
        CycleThemeEventHandler(IntentSink intentSink, MenuState& state);

        [[nodiscard]] bool isHandled(ftxui::Event event) const override;
        void handleEvent(ftxui::Event event) override;

    private:
        IntentSink intentSink_;
        MenuState& state_;
    };

    /** @brief Leaves the game. */
    class QuitEventHandler final : public EventHandler {
    public:
        explicit QuitEventHandler(IntentSink intentSink);

        [[nodiscard]] bool isHandled(ftxui::Event event) const override;
        void handleEvent(ftxui::Event event) override;

    private:
        IntentSink intentSink_;
    };
} // namespace cpp_warships::game_tui
