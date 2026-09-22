#include <game_tui/screens/PlacementScreen.h>
#include <game_tui/screens/PlacementEventHandlers.h>

#include <ftxui/component/event.hpp>
#include <memory>
#include <utility>

namespace cpp_warships::game_tui {
    PlacementController::PlacementController(
            IntentSink intentSink,
            const Theme& theme,
            MatchQuery match
    )
        : theme_(theme)
        , match_(std::move(match)) {
        eventRouter_.add(
                std::make_shared<PlacementMouseEventHandler>(intentSink, state_, match_, view_)
        );
        eventRouter_.add(std::make_shared<MoveCursorEventHandler>(state_, match_));
        eventRouter_.add(std::make_shared<RotateShipEventHandler>(state_));
        eventRouter_.add(std::make_shared<CycleShipLengthEventHandler>(state_, match_));
        eventRouter_.add(std::make_shared<PlaceShipEventHandler>(intentSink, state_, match_));
        eventRouter_.add(std::make_shared<RemoveShipEventHandler>(intentSink, state_));
        eventRouter_.add(std::make_shared<ShuffleFleetEventHandler>(intentSink));
        eventRouter_.add(std::make_shared<BeginBattleEventHandler>(intentSink, match_));
        eventRouter_.add(std::make_shared<LeavePlacementEventHandler>(intentSink));
    }

    ftxui::Component PlacementController::component() {
        const auto renderPlacement = [this] {
            return view_.render(theme_, match_(), state_);
        };

        const auto dispatchEvent = [this](ftxui::Event event) {
            return eventRouter_.dispatch(event);
        };

        return ftxui::CatchEvent(ftxui::Renderer(renderPlacement), dispatchEvent);
    }

    PlacementScreen::PlacementScreen(IntentSink intentSink, const Theme& theme, MatchQuery match)
        : controller_(std::move(intentSink), theme, std::move(match))
        , component_(controller_.component()) {}

    ScreenKind PlacementScreen::kind() const {
        return ScreenKind::Placement;
    }

    ftxui::Component PlacementScreen::component() {
        return component_;
    }
} // namespace cpp_warships::game_tui
