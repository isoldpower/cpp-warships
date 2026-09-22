#include <utility>

#include <game_tui/Application.h>
#include <game_tui/intents/BeginBattleIntent.h>
#include <game_tui/intents/ChangeThemeIntent.h>
#include <game_tui/intents/PlaceShipIntent.h>
#include <game_tui/intents/QuitIntent.h>
#include <game_tui/intents/RemoveShipIntent.h>
#include <game_tui/intents/ShowScreenIntent.h>
#include <game_tui/intents/ShuffleFleetIntent.h>
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

    PlaceShipIntent::PlaceShipIntent(
            game_core::Coordinate origin,
            game_core::Direction direction,
            int length
    )
        : origin_(origin)
        , direction_(direction)
        , length_(length) {}

    void PlaceShipIntent::applyTo(Application& application) const {
        application.placeShip(origin_, direction_, length_);
    }

    RemoveShipIntent::RemoveShipIntent(game_core::Coordinate coordinate)
        : coordinate_(coordinate) {}

    void RemoveShipIntent::applyTo(Application& application) const {
        application.removeShipAt(coordinate_);
    }

    void ShuffleFleetIntent::applyTo(Application& application) const {
        application.shuffleFleet();
    }

    void BeginBattleIntent::applyTo(Application& application) const {
        application.beginBattle();
    }

    void QuitIntent::applyTo(Application& application) const {
        application.requestQuit();
    }
} // namespace cpp_warships::game_tui
