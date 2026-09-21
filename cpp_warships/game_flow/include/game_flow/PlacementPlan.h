#pragma once

#include <map>

#include <game_core/Board.h>
#include <game_core/FleetComposition.h>
#include <game_flow/RandomEngine.h>

namespace cpp_warships::game_flow {
    /** @brief What still has to be placed to satisfy a fleet composition.
     *  Derived from the board on demand, so the two cannot drift apart. */
    class PlacementPlan {
    public:
        PlacementPlan(game_core::FleetComposition composition, const game_core::Board& board);

        [[nodiscard]] int remainingOf(int shipLength) const;
        [[nodiscard]] const std::map<int, int>& remaining() const noexcept;
        [[nodiscard]] int remainingShipCount() const;
        [[nodiscard]] bool isComplete() const;

    private:
        std::map<int, int> remaining_;
    };

    /** @brief Clears the board and lays out a whole fleet at random.
     *  @return false when no legal layout was found, leaving the board empty. */
    bool placeFleetRandomly(
            game_core::Board& board,
            const game_core::FleetComposition& composition,
            RandomEngine& randomEngine
    );
} // namespace cpp_warships::game_flow
