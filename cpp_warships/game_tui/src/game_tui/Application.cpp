#include <game_tui/Application.h>

#include <stdexcept>
#include <utility>

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <game_core/MatchSettings.h>
#include <game_tui/MatchQuery.h>
#include <game_tui/screens/MenuScreen.h>
#include <game_tui/screens/PlacementScreen.h>

namespace cpp_warships::game_tui {

    Application::Application(game_flow::RandomEngine& randomEngine)
        : randomEngine_(randomEngine)
        , theme_(defaultTheme())
        , currentScreen_(ScreenKind::Menu)
        , interactiveScreen_(nullptr) {
        const IntentSink intentSink = [this](const IntentPointer& intent) {
            submit(intent);
        };

        const auto hasMatchInProgress = [this] {
            return hasMatch();
        };

        const MatchQuery matchInPlay = [this]() -> const game_flow::Match& {
            return match();
        };

        screens_[ScreenKind::Menu] =
                std::make_unique<MenuScreen>(intentSink, theme_, hasMatchInProgress);
        screens_[ScreenKind::Placement] =
                std::make_unique<PlacementScreen>(intentSink, theme_, matchInPlay);
    }

    Application::~Application() = default;

    const Theme& Application::theme() const noexcept {
        return theme_;
    }

    ScreenKind Application::currentScreen() const noexcept {
        return currentScreen_;
    }

    bool Application::hasMatch() const noexcept {
        return match_.has_value();
    }

    game_flow::Match& Application::match() {
        if (!match_.has_value()) {
            throw std::logic_error("Application::match called with no match in play");
        }

        return *match_;
    }

    const game_flow::Match& Application::match() const {
        if (!match_.has_value()) {
            throw std::logic_error("Application::match called with no match in play");
        }

        return *match_;
    }

    void Application::showScreen(ScreenKind screen) {
        if (screens_.contains(screen)) {
            currentScreen_ = screen;
        }
    }

    void Application::startNewMatch(int boardSize) {
        match_.emplace(game_core::MatchSettings::forBoardSize(boardSize), randomEngine_);
        showScreen(ScreenKind::Placement);
    }

    void Application::placeShip(
            game_core::Coordinate origin,
            game_core::Direction direction,
            int length
    ) {
        if (match_.has_value()) {
            match_->editablePlayerBoard()
                    .place(origin, direction, length, match_->settings().segmentHealth());
        }
    }

    void Application::removeShipAt(game_core::Coordinate coordinate) {
        if (match_.has_value()) {
            match_->editablePlayerBoard().removeShipAt(coordinate);
        }
    }

    void Application::shuffleFleet() {
        if (match_.has_value()) {
            match_->shufflePlayerFleet();
        }
    }

    void Application::beginBattle() {
        if (match_.has_value() && match_->beginBattle()) {
            showScreen(ScreenKind::Battle);
        }
    }

    void Application::changeTheme(const std::string& themeName) {
        theme_ = themeNamed(themeName);
    }

    void Application::requestQuit() {
        if (interactiveScreen_ != nullptr) {
            interactiveScreen_->Exit();
        }
    }

    void Application::submit(const IntentPointer& intent) {
        if (intent) {
            intent->applyTo(*this);
        }
    }

    Screen& Application::activeScreen() {
        return *screens_.at(currentScreen_);
    }

    void Application::run() {
        const auto renderActiveScreen = [this] {
            return activeScreen().component()->Render() | ftxui::bgcolor(theme_.background);
        };

        const auto routeEvent = [this](ftxui::Event event) {
            return activeScreen().component()->OnEvent(event);
        };

        ftxui::ScreenInteractive screenInteractive = ftxui::ScreenInteractive::Fullscreen();
        interactiveScreen_ = &screenInteractive;

        const ftxui::Component root =
                ftxui::CatchEvent(ftxui::Renderer(renderActiveScreen), routeEvent);
        screenInteractive.Loop(root);

        interactiveScreen_ = nullptr;
    }
} // namespace cpp_warships::game_tui
