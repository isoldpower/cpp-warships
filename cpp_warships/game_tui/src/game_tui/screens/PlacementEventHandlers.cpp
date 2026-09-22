#include <game_tui/screens/PlacementEventHandlers.h>

#include <algorithm>
#include <memory>
#include <optional>
#include <string>
#include <utility>

#include <ftxui/component/mouse.hpp>

#include <game_tui/ScreenKind.h>
#include <game_tui/intents/BeginBattleIntent.h>
#include <game_tui/intents/PlaceShipIntent.h>
#include <game_tui/intents/RemoveShipIntent.h>
#include <game_tui/intents/ShowScreenIntent.h>
#include <game_tui/intents/ShuffleFleetIntent.h>
#include <game_tui/screens/PlacementView.h>

namespace cpp_warships::game_tui {
    namespace {
        bool isCharacter(const ftxui::Event& event, const std::string& character) {
            return event.is_character() && event.character() == character;
        }

        game_core::Coordinate stepOf(const ftxui::Event& event) {
            if (event == ftxui::Event::ArrowLeft) {
                return {-1, 0};
            }
            if (event == ftxui::Event::ArrowRight) {
                return {1, 0};
            }
            if (event == ftxui::Event::ArrowUp) {
                return {0, -1};
            }

            return {0, 1};
        }

        game_core::Direction turnedFrom(game_core::Direction direction) {
            return direction == game_core::Direction::Horizontal ? game_core::Direction::Vertical
                                                                 : game_core::Direction::Horizontal;
        }
    } // namespace

    MoveCursorEventHandler::MoveCursorEventHandler(PlacementState& state, MatchQuery match)
        : state_(state)
        , match_(std::move(match)) {}

    bool MoveCursorEventHandler::isHandled(ftxui::Event event) const {
        return event == ftxui::Event::ArrowLeft || event == ftxui::Event::ArrowRight ||
               event == ftxui::Event::ArrowUp || event == ftxui::Event::ArrowDown;
    }

    void MoveCursorEventHandler::handleEvent(ftxui::Event event) {
        const game_core::Board& board = match_().playerBoard();
        const game_core::Coordinate step = stepOf(event);

        state_.cursor.x = std::clamp(state_.cursor.x + step.x, 0, board.width() - 1);
        state_.cursor.y = std::clamp(state_.cursor.y + step.y, 0, board.height() - 1);
    }

    RotateShipEventHandler::RotateShipEventHandler(PlacementState& state)
        : state_(state) {}

    bool RotateShipEventHandler::isHandled(ftxui::Event event) const {
        return isCharacter(event, "r");
    }

    void RotateShipEventHandler::handleEvent(ftxui::Event) {
        state_.direction = turnedFrom(state_.direction);
    }

    CycleShipLengthEventHandler::CycleShipLengthEventHandler(
            PlacementState& state,
            MatchQuery match
    )
        : state_(state)
        , match_(std::move(match)) {}

    bool CycleShipLengthEventHandler::isHandled(ftxui::Event event) const {
        return event == ftxui::Event::Tab;
    }

    void CycleShipLengthEventHandler::handleEvent(ftxui::Event) {
        const game_flow::PlacementPlan plan = match_().playerPlacementPlan();
        state_.preferredShipLength = nextShipLength(plan, shipLengthInHand(plan, state_));
    }

    PlaceShipEventHandler::PlaceShipEventHandler(
            IntentSink intentSink,
            const PlacementState& state,
            MatchQuery match
    )
        : intentSink_(std::move(intentSink))
        , state_(state)
        , match_(std::move(match)) {}

    bool PlaceShipEventHandler::isHandled(ftxui::Event event) const {
        return event == ftxui::Event::Return;
    }

    void PlaceShipEventHandler::handleEvent(ftxui::Event) {
        const game_flow::PlacementPlan plan = match_().playerPlacementPlan();
        const int lengthInHand = shipLengthInHand(plan, state_);
        if (lengthInHand <= 0) {
            return;
        }

        intentSink_(
                std::make_shared<PlaceShipIntent>(state_.cursor, state_.direction, lengthInHand)
        );
    }

    RemoveShipEventHandler::RemoveShipEventHandler(
            IntentSink intentSink,
            const PlacementState& state
    )
        : intentSink_(std::move(intentSink))
        , state_(state) {}

    bool RemoveShipEventHandler::isHandled(ftxui::Event event) const {
        return event == ftxui::Event::Backspace || event == ftxui::Event::Delete;
    }

    void RemoveShipEventHandler::handleEvent(ftxui::Event) {
        intentSink_(std::make_shared<RemoveShipIntent>(state_.cursor));
    }

    ShuffleFleetEventHandler::ShuffleFleetEventHandler(IntentSink intentSink)
        : intentSink_(std::move(intentSink)) {}

    bool ShuffleFleetEventHandler::isHandled(ftxui::Event event) const {
        return isCharacter(event, "s");
    }

    void ShuffleFleetEventHandler::handleEvent(ftxui::Event) {
        intentSink_(std::make_shared<ShuffleFleetIntent>());
    }

    BeginBattleEventHandler::BeginBattleEventHandler(IntentSink intentSink, MatchQuery match)
        : intentSink_(std::move(intentSink))
        , match_(std::move(match)) {}

    bool BeginBattleEventHandler::isHandled(ftxui::Event event) const {
        return isCharacter(event, "b") && match_().playerPlacementPlan().isComplete();
    }

    void BeginBattleEventHandler::handleEvent(ftxui::Event) {
        intentSink_(std::make_shared<BeginBattleIntent>());
    }

    PlacementMouseEventHandler::PlacementMouseEventHandler(
            IntentSink intentSink,
            PlacementState& state,
            MatchQuery match,
            const PlacementView& view
    )
        : intentSink_(std::move(intentSink))
        , state_(state)
        , match_(std::move(match))
        , view_(view) {}

    bool PlacementMouseEventHandler::isHandled(ftxui::Event event) const {
        return event.is_mouse() && view_.cellAt(event.mouse().x, event.mouse().y).has_value();
    }

    void PlacementMouseEventHandler::handleEvent(ftxui::Event event) {
        const ftxui::Mouse& mouse = event.mouse();
        const std::optional<game_core::Coordinate> cell = view_.cellAt(mouse.x, mouse.y);
        if (!cell.has_value()) {
            return;
        }

        state_.cursor = *cell;

        if (mouse.button == ftxui::Mouse::WheelUp || mouse.button == ftxui::Mouse::WheelDown) {
            const game_flow::PlacementPlan plan = match_().playerPlacementPlan();
            state_.preferredShipLength = nextShipLength(plan, shipLengthInHand(plan, state_));
            return;
        }

        if (mouse.motion != ftxui::Mouse::Pressed) {
            return;
        }

        if (mouse.button == ftxui::Mouse::Right) {
            intentSink_(std::make_shared<RemoveShipIntent>(*cell));
            return;
        }

        if (mouse.button == ftxui::Mouse::Left) {
            const game_flow::PlacementPlan plan = match_().playerPlacementPlan();
            const int lengthInHand = shipLengthInHand(plan, state_);
            if (lengthInHand > 0) {
                intentSink_(
                        std::make_shared<PlaceShipIntent>(*cell, state_.direction, lengthInHand)
                );
            }
        }
    }

    LeavePlacementEventHandler::LeavePlacementEventHandler(IntentSink intentSink)
        : intentSink_(std::move(intentSink)) {}

    bool LeavePlacementEventHandler::isHandled(ftxui::Event event) const {
        return event == ftxui::Event::Escape;
    }

    void LeavePlacementEventHandler::handleEvent(ftxui::Event) {
        intentSink_(std::make_shared<ShowScreenIntent>(ScreenKind::Menu));
    }
} // namespace cpp_warships::game_tui
