#pragma once

#include <optional>
#include <unordered_set>
#include <vector>

#include <game_core/Board.h>
#include <game_core/Coordinate.h>
#include <game_flow/RandomEngine.h>

namespace cpp_warships::game_flow {
    /** @brief Chooses where the computer shoots: at random, then along any ship it finds.
     *  Randomness comes from the injected engine, so a seed reproduces a whole game. */
    class AiOpponent {
    public:
        explicit AiOpponent(RandomEngine& randomEngine);

        /** @brief Picks the next cell to attack on @p board.
         *  @return nullopt when every cell has already been attempted. */
        [[nodiscard]] std::optional<game_core::Coordinate> chooseTarget(
                const game_core::Board& board
        );

        /** @brief Feeds back what the chosen shot did, shaping the next choice.
         *  Delegates to the outcome's behaviour rather than branching on it here. */
        void recordOutcome(
                game_core::Coordinate coordinate,
                game_core::AttackOutcome outcome,
                const game_core::Board& board
        );

        /** @brief Notes that @p coordinate has been fired at and need not be tried again. */
        void markAttempted(game_core::Coordinate coordinate);

        /** @brief Adds a hit to the ship currently being chased. */
        void registerHit(game_core::Coordinate coordinate);

        /** @brief Ends the current chase, ruling out every cell around the sunk ship. */
        void finishHunt(const game_core::Board& board);

        void reset();

    private:
        [[nodiscard]] std::vector<game_core::Coordinate> untriedNeighbours(
                game_core::Coordinate coordinate,
                const game_core::Board& board
        ) const;
        [[nodiscard]] std::optional<game_core::Coordinate> continueAlongHits(
                const game_core::Board& board
        ) const;
        [[nodiscard]] std::optional<game_core::Coordinate> pickRandomUntried(
                const game_core::Board& board
        );

        RandomEngine& randomEngine_;
        std::unordered_set<game_core::Coordinate> attemptedCoordinates_;
        std::vector<game_core::Coordinate> currentTargetHits_;
    };
} // namespace cpp_warships::game_flow
