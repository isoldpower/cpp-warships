#pragma once

#include <game_core/Board.h>
#include <game_core/Coordinate.h>
#include <game_core/Outcomes.h>
#include <game_flow/MatchEvent.h>

namespace cpp_warships::game_flow {
    class AiOpponent;

    /** @brief What a shot's outcome means for the turn, the event log and the hunt.
     *  One implementation per outcome, so the rules live together instead of in switches. */
    class AttackOutcomeBehaviour {
    public:
        virtual ~AttackOutcomeBehaviour() = default;

        /** @brief Whether the shooter fires again. */
        [[nodiscard]] virtual bool keepsTurn() const = 0;

        /** @brief Whether the shot was actually taken, spending any armed bonus. */
        [[nodiscard]] virtual bool isShotSpent() const = 0;

        /** @brief Whether sinking earned the shooter a skill. */
        [[nodiscard]] virtual bool grantsSkill() const = 0;

        /** @brief The event this outcome appends to the match history. */
        [[nodiscard]] virtual MatchEventKind eventKind() const = 0;

        /** @brief Folds the result into the computer's hunt for the ship it is chasing. */
        virtual void updateHunt(
                AiOpponent& opponent,
                game_core::Coordinate coordinate,
                const game_core::Board& board
        ) const = 0;
    };

    /** @brief The behaviour describing @p outcome.
     *  Returns a shared, stateless instance; never null. */
    const AttackOutcomeBehaviour& behaviourFor(game_core::AttackOutcome outcome);
} // namespace cpp_warships::game_flow
