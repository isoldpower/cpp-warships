#pragma once

#include <application/core/Coordinate.h>
#include <application/core/Outcomes.h>
#include <application/flow/RandomEngine.h>

#include <optional>
#include <unordered_set>
#include <vector>

namespace cpp_warships::core {
    class Board;
}

namespace cpp_warships::flow {
    /** @brief What the computer has learned so far: where it has already fired,
     * and the hits on the ship it is currently chasing. */
    struct AiMemory {
        std::unordered_set<core::Coordinate> attemptedCoordinates;
        std::vector<core::Coordinate> currentTargetHits;
    };

    /** @brief Chooses where the computer shoots: at random, then along any ship
     * it finds. Randomness comes from the injected engine, so a seed reproduces a whole game. */
    class AiOpponent {
    public:
        explicit AiOpponent(RandomEngine& randomEngine);

        /** @brief An opponent that already knows @p memory, as when a save is loaded. */
        AiOpponent(RandomEngine& randomEngine, AiMemory memory);

        /** @brief Everything this opponent has learned, ready to be written out. */
        [[nodiscard]] AiMemory memory() const;

        /** @brief Picks the next cell to attack on @p board.
         * @return nullopt when every cell has already been attempted. */
        [[nodiscard]] std::optional<core::Coordinate> chooseTarget(const core::Board& board);

        /** @brief Feeds back what the chosen shot did, shaping the next choice.
         * Delegates to the outcome's behaviour rather than branching on it here. */
        void recordOutcome(
            core::Coordinate coordinate,
            core::AttackOutcome outcome,
            const core::Board& board
        );

        /** @brief Notes that @p coordinate has been fired at and need not be
         * tried again. */
        void markAttempted(core::Coordinate coordinate);

        /** @brief Adds a hit to the ship currently being chased. */
        void registerHit(core::Coordinate coordinate);

        /** @brief Ends the current chase, ruling out every cell around the sunk
         * ship. */
        void finishHunt(const core::Board& board);

    private:
        /** @brief A cell already struck that is still holding, and so is worth striking again.
         * A segment outlasts a single shot, so the chase must finish one before moving on. */
        [[nodiscard]] std::optional<core::Coordinate> unfinishedHit(const core::Board& board) const;
        /** @brief A random untried cell beside the one hit so far, when exactly one is. */
        [[nodiscard]] std::optional<core::Coordinate> besideTheOnlyHit(const core::Board& board);

        [[nodiscard]] std::vector<core::Coordinate> untriedNeighbours(
            core::Coordinate coordinate,
            const core::Board& board
        ) const;
        /** @brief The next untried cell at either end of a run of hits, when there are two or more.
         */
        [[nodiscard]] std::optional<core::Coordinate> continueAlongHits(
            const core::Board& board
        ) const;
        [[nodiscard]] std::optional<core::Coordinate> pickRandomUntried(const core::Board& board);

        RandomEngine& randomEngine_;
        std::unordered_set<core::Coordinate> attemptedCoordinates_;
        std::vector<core::Coordinate> currentTargetHits_;
    };
}  // namespace cpp_warships::flow
