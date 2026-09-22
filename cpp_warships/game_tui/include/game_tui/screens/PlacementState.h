#pragma once

#include <game_core/Coordinate.h>
#include <game_core/Direction.h>
#include <game_flow/PlacementPlan.h>

namespace cpp_warships::game_tui {
    /** @brief Where the player is aiming on their board and how the ship in hand lies. */
    struct PlacementState {
        game_core::Coordinate cursor;
        game_core::Direction direction = game_core::Direction::Horizontal;
        int preferredShipLength = 0;
    };

    /** @brief The length in hand: the player's pick while any of it is left, else the longest. */
    [[nodiscard]] int shipLengthInHand(
            const game_flow::PlacementPlan& plan,
            const PlacementState& state
    );

    /** @brief The next longer length still waiting to be placed, wrapping to the shortest. */
    [[nodiscard]] int nextShipLength(const game_flow::PlacementPlan& plan, int currentLength);
} // namespace cpp_warships::game_tui
