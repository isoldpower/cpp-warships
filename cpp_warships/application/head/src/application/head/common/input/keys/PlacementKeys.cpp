#include <application/head/common/PresentationContext.h>
#include <application/head/common/input/Keystroke.h>
#include <application/head/common/input/keys/PlacementKeys.h>

namespace cpp_warships::head::common::input::keys {
    namespace {
        [[nodiscard]] core::Direction turnedFrom(const core::Direction direction) {
            return direction == core::Direction::Horizontal ? core::Direction::Vertical
                                                            : core::Direction::Horizontal;
        }
    }  // namespace

    std::optional<model::events::GameEvent> layShipInHand(const PresentationContext& context) {
        const state::PlacementState& state = context.state().placement;
        const int lengthInHand =
            state::shipLengthInHand(context.game().match().playerPlacementPlan(), state);
        if (lengthInHand <= 0) {
            return std::nullopt;
        }

        return model::events::ShipPlacementRequested{
            .origin = state.cursor,
            .direction = state.direction,
            .length = lengthInHand
        };
    }

    void pickNextShipLength(PresentationContext& context) {
        const flow::PlacementPlan& plan = context.game().match().playerPlacementPlan();
        state::PlacementState& state = context.state().placement;

        state.preferredShipLength =
            state::nextShipLength(plan, state::shipLengthInHand(plan, state));
    }

    PlacementKey::PlacementKey(PresentationContext& context) noexcept
        : context_(context) {}

    std::optional<core::Coordinate> PlacementKey::cellUnderPointer(const Keystroke& stroke) const {
        if (!isPointer(stroke)) {
            return std::nullopt;
        }

        return context_.geometry()
            .cellAt(ScreenRegion::OwnWaters, stroke.pointerX, stroke.pointerY);
    }

    core::Coordinate& MovePlacementCursorKey::cursor() {
        return context_.state().placement.cursor;
    }

    const core::Board& MovePlacementCursorKey::board() const {
        return context_.game().match().playerBoard();
    }

    ScreenRegion MovePlacementCursorKey::region() const {
        return ScreenRegion::OwnWaters;
    }

    bool TurnShipKey::matches(const Keystroke& stroke) const {
        return isCharacter(stroke, "r");
    }

    std::optional<model::events::GameEvent> TurnShipKey::interpret(const Keystroke&) {
        state::PlacementState& state = context_.state().placement;
        state.direction = turnedFrom(state.direction);

        return std::nullopt;
    }

    bool PickShipLengthKey::matches(const Keystroke& stroke) const {
        return stroke.key == Key::Tab;
    }

    std::optional<model::events::GameEvent> PickShipLengthKey::interpret(const Keystroke&) {
        pickNextShipLength(context_);
        return std::nullopt;
    }

    bool LayShipKey::matches(const Keystroke& stroke) const {
        return stroke.key == Key::Enter;
    }

    std::optional<model::events::GameEvent> LayShipKey::interpret(const Keystroke&) {
        return layShipInHand(context_);
    }

    bool TakeShipBackKey::matches(const Keystroke& stroke) const {
        return stroke.key == Key::Backspace || stroke.key == Key::Delete;
    }

    std::optional<model::events::GameEvent> TakeShipBackKey::interpret(const Keystroke&) {
        return model::events::ShipRemovalRequested{.coordinate = context_.state().placement.cursor};
    }

    bool ShuffleFleetKey::matches(const Keystroke& stroke) const {
        return isCharacter(stroke, "f");
    }

    std::optional<model::events::GameEvent> ShuffleFleetKey::interpret(const Keystroke&) {
        return model::events::FleetShuffleRequested{};
    }

    bool BeginBattleKey::matches(const Keystroke& stroke) const {
        return isCharacter(stroke, "b");
    }

    std::optional<model::events::GameEvent> BeginBattleKey::interpret(const Keystroke&) {
        return model::events::BattleBeginRequested{};
    }

    bool LayShipWithPointerKey::matches(const Keystroke& stroke) const {
        return cellUnderPointer(stroke).has_value() && stroke.isPressed &&
               stroke.button == PointerButton::Left;
    }

    std::optional<model::events::GameEvent> LayShipWithPointerKey::interpret(
        const Keystroke& stroke
    ) {
        context_.state().placement.cursor = *cellUnderPointer(stroke);
        return layShipInHand(context_);
    }

    bool TakeShipBackWithPointerKey::matches(const Keystroke& stroke) const {
        return cellUnderPointer(stroke).has_value() && stroke.isPressed &&
               stroke.button == PointerButton::Right;
    }

    std::optional<model::events::GameEvent> TakeShipBackWithPointerKey::interpret(
        const Keystroke& stroke
    ) {
        const core::Coordinate cell = *cellUnderPointer(stroke);
        context_.state().placement.cursor = cell;

        return model::events::ShipRemovalRequested{.coordinate = cell};
    }

    bool AimWithPointerKey::matches(const Keystroke& stroke) const {
        return cellUnderPointer(stroke).has_value();
    }

    std::optional<model::events::GameEvent> AimWithPointerKey::interpret(const Keystroke& stroke) {
        context_.state().placement.cursor = *cellUnderPointer(stroke);
        return std::nullopt;
    }
}  // namespace cpp_warships::head::common::input::keys
