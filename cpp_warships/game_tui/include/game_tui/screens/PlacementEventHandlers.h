#pragma once

#include <game_tui/EventHandler.h>
#include <game_tui/Intent.h>
#include <game_tui/MatchQuery.h>
#include <game_tui/screens/PlacementState.h>

namespace cpp_warships::game_tui {
    class PlacementView;

    /** @brief Walks the aiming cursor around the board with the arrow keys. */
    class MoveCursorEventHandler final : public EventHandler {
    public:
        MoveCursorEventHandler(PlacementState& state, MatchQuery match);

        [[nodiscard]] bool isHandled(ftxui::Event event) const override;
        void handleEvent(ftxui::Event event) override;

    private:
        PlacementState& state_;
        MatchQuery match_;
    };

    /** @brief Turns the ship in hand between lying across and lying down. */
    class RotateShipEventHandler final : public EventHandler {
    public:
        explicit RotateShipEventHandler(PlacementState& state);

        [[nodiscard]] bool isHandled(ftxui::Event event) const override;
        void handleEvent(ftxui::Event event) override;

    private:
        PlacementState& state_;
    };

    /** @brief Picks up the next length of ship still waiting to be placed. */
    class CycleShipLengthEventHandler final : public EventHandler {
    public:
        CycleShipLengthEventHandler(PlacementState& state, MatchQuery match);

        [[nodiscard]] bool isHandled(ftxui::Event event) const override;
        void handleEvent(ftxui::Event event) override;

    private:
        PlacementState& state_;
        MatchQuery match_;
    };

    /** @brief Lays the ship in hand at the cursor, leaving the board to refuse an illegal spot. */
    class PlaceShipEventHandler final : public EventHandler {
    public:
        PlaceShipEventHandler(IntentSink intentSink, const PlacementState& state, MatchQuery match);

        [[nodiscard]] bool isHandled(ftxui::Event event) const override;
        void handleEvent(ftxui::Event event) override;

    private:
        IntentSink intentSink_;
        const PlacementState& state_;
        MatchQuery match_;
    };

    /** @brief Takes back the ship lying under the cursor. */
    class RemoveShipEventHandler final : public EventHandler {
    public:
        RemoveShipEventHandler(IntentSink intentSink, const PlacementState& state);

        [[nodiscard]] bool isHandled(ftxui::Event event) const override;
        void handleEvent(ftxui::Event event) override;

    private:
        IntentSink intentSink_;
        const PlacementState& state_;
    };

    /** @brief Throws the whole fleet back onto the board at random. */
    class ShuffleFleetEventHandler final : public EventHandler {
    public:
        explicit ShuffleFleetEventHandler(IntentSink intentSink);

        [[nodiscard]] bool isHandled(ftxui::Event event) const override;
        void handleEvent(ftxui::Event event) override;

    private:
        IntentSink intentSink_;
    };

    /** @brief Opens fire, and only offers itself once every ship is on the board. */
    class BeginBattleEventHandler final : public EventHandler {
    public:
        BeginBattleEventHandler(IntentSink intentSink, MatchQuery match);

        [[nodiscard]] bool isHandled(ftxui::Event event) const override;
        void handleEvent(ftxui::Event event) override;

    private:
        IntentSink intentSink_;
        MatchQuery match_;
    };

    /** @brief Aims, lays and takes back ships with the mouse, reading cells off the drawn grid. */
    class PlacementMouseEventHandler final : public EventHandler {
    public:
        PlacementMouseEventHandler(
                IntentSink intentSink,
                PlacementState& state,
                MatchQuery match,
                const PlacementView& view
        );

        [[nodiscard]] bool isHandled(ftxui::Event event) const override;
        void handleEvent(ftxui::Event event) override;

    private:
        IntentSink intentSink_;
        PlacementState& state_;
        MatchQuery match_;
        const PlacementView& view_;
    };

    /** @brief Goes back to the menu, leaving the fleet as it stands. */
    class LeavePlacementEventHandler final : public EventHandler {
    public:
        explicit LeavePlacementEventHandler(IntentSink intentSink);

        [[nodiscard]] bool isHandled(ftxui::Event event) const override;
        void handleEvent(ftxui::Event event) override;

    private:
        IntentSink intentSink_;
    };
} // namespace cpp_warships::game_tui
