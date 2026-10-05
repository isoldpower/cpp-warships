#pragma once

#include <application/head/common/input/InputKey.h>
#include <application/head/common/input/keys/MoveCursorKey.h>

namespace cpp_warships::head::common {
    class PresentationContext;
}

namespace cpp_warships::head::common::input::keys {
    /** @brief The ask to lay the ship in hand where the cursor rests, if one is in hand. */
    [[nodiscard]] std::optional<model::events::GameEvent> layShipInHand(
        const PresentationContext& context
    );

    /** @brief Moves the preference on to the next length still waiting to be placed. */
    void pickNextShipLength(PresentationContext& context);

    /** @brief Every binding here works from the context and nothing else. */
    class PlacementKey : public InputKey {
    public:
        explicit PlacementKey(PresentationContext& context) noexcept;

    protected:
        /** @brief The cell the pointer is over, when it is over the player's own board. */
        [[nodiscard]] std::optional<core::Coordinate> cellUnderPointer(
            const Keystroke& stroke
        ) const;

        PresentationContext& context_;
    };

    /** @brief Walks the aiming cursor around the player's own board. */
    class MovePlacementCursorKey final : public MoveCursorKey {
    public:
        using MoveCursorKey::MoveCursorKey;

    protected:
        [[nodiscard]] core::Coordinate& cursor() override;
        [[nodiscard]] const core::Board& board() const override;
        [[nodiscard]] ScreenRegion region() const override;
    };

    /** @brief Turns the ship in hand from lying across to lying down and back. */
    class TurnShipKey final : public PlacementKey {
    public:
        using PlacementKey::PlacementKey;

        [[nodiscard]] bool matches(const Keystroke& stroke) const override;
        [[nodiscard]] std::optional<model::events::GameEvent> interpret(
            const Keystroke& stroke
        ) override;
    };

    /** @brief Takes up the next length still waiting to be placed. */
    class PickShipLengthKey final : public PlacementKey {
    public:
        using PlacementKey::PlacementKey;

        [[nodiscard]] bool matches(const Keystroke& stroke) const override;
        [[nodiscard]] std::optional<model::events::GameEvent> interpret(
            const Keystroke& stroke
        ) override;
    };

    /** @brief Lays the ship in hand where the cursor rests. */
    class LayShipKey final : public PlacementKey {
    public:
        using PlacementKey::PlacementKey;

        [[nodiscard]] bool matches(const Keystroke& stroke) const override;
        [[nodiscard]] std::optional<model::events::GameEvent> interpret(
            const Keystroke& stroke
        ) override;
    };

    /** @brief Takes back whichever ship covers the cursor. */
    class TakeShipBackKey final : public PlacementKey {
    public:
        using PlacementKey::PlacementKey;

        [[nodiscard]] bool matches(const Keystroke& stroke) const override;
        [[nodiscard]] std::optional<model::events::GameEvent> interpret(
            const Keystroke& stroke
        ) override;
    };

    /** @brief Lays the whole fleet out at random. */
    class ShuffleFleetKey final : public InputKey {
    public:
        [[nodiscard]] bool matches(const Keystroke& stroke) const override;
        [[nodiscard]] std::optional<model::events::GameEvent> interpret(
            const Keystroke& stroke
        ) override;
    };

    /** @brief Stops laying out and opens fire. */
    class BeginBattleKey final : public InputKey {
    public:
        [[nodiscard]] bool matches(const Keystroke& stroke) const override;
        [[nodiscard]] std::optional<model::events::GameEvent> interpret(
            const Keystroke& stroke
        ) override;
    };

    /** @brief Lays the ship in hand where the pointer was pressed. */
    class LayShipWithPointerKey final : public PlacementKey {
    public:
        using PlacementKey::PlacementKey;

        [[nodiscard]] bool matches(const Keystroke& stroke) const override;
        [[nodiscard]] std::optional<model::events::GameEvent> interpret(
            const Keystroke& stroke
        ) override;
    };

    /** @brief Takes back whichever ship the pointer was pressed on. */
    class TakeShipBackWithPointerKey final : public PlacementKey {
    public:
        using PlacementKey::PlacementKey;

        [[nodiscard]] bool matches(const Keystroke& stroke) const override;
        [[nodiscard]] std::optional<model::events::GameEvent> interpret(
            const Keystroke& stroke
        ) override;
    };

    /** @brief Moves the cursor to wherever the pointer is, asking the game for nothing. */
    class AimWithPointerKey final : public PlacementKey {
    public:
        using PlacementKey::PlacementKey;

        [[nodiscard]] bool matches(const Keystroke& stroke) const override;
        [[nodiscard]] std::optional<model::events::GameEvent> interpret(
            const Keystroke& stroke
        ) override;
    };
}  // namespace cpp_warships::head::common::input::keys
