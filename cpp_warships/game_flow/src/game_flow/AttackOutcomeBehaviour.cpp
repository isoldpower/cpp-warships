#include <game_flow/AttackOutcomeBehaviour.h>

#include <array>
#include <cstddef>

#include <game_flow/AiOpponent.h>

namespace cpp_warships::game_flow {
    namespace {
        /** @brief A shot that found open water: the turn passes to the opponent. */
        class MissBehaviour final : public AttackOutcomeBehaviour {
        public:
            [[nodiscard]] bool keepsTurn() const override {
                return false;
            }

            [[nodiscard]] bool isShotSpent() const override {
                return true;
            }

            [[nodiscard]] bool grantsSkill() const override {
                return false;
            }

            [[nodiscard]] MatchEventKind eventKind() const override {
                return MatchEventKind::ShotMissed;
            }

            void updateHunt(
                    AiOpponent&,
                    game_core::Coordinate,
                    const game_core::Board&
            ) const override {
                // A miss tells the hunt nothing beyond the cell already being marked.
            }
        };

        /** @brief A shot that wounded a ship without finishing it: the shooter fires again. */
        class HitBehaviour final : public AttackOutcomeBehaviour {
        public:
            [[nodiscard]] bool keepsTurn() const override {
                return true;
            }

            [[nodiscard]] bool isShotSpent() const override {
                return true;
            }

            [[nodiscard]] bool grantsSkill() const override {
                return false;
            }

            [[nodiscard]] MatchEventKind eventKind() const override {
                return MatchEventKind::ShipDamaged;
            }

            void updateHunt(
                    AiOpponent& opponent,
                    game_core::Coordinate coordinate,
                    const game_core::Board&
            ) const override {
                opponent.registerHit(coordinate);
            }
        };

        /** @brief A shot that sank a ship: the shooter fires again and earns a skill. */
        class SunkBehaviour final : public AttackOutcomeBehaviour {
        public:
            [[nodiscard]] bool keepsTurn() const override {
                return true;
            }

            [[nodiscard]] bool isShotSpent() const override {
                return true;
            }

            [[nodiscard]] bool grantsSkill() const override {
                return true;
            }

            [[nodiscard]] MatchEventKind eventKind() const override {
                return MatchEventKind::ShipSunk;
            }

            void updateHunt(
                    AiOpponent& opponent,
                    game_core::Coordinate coordinate,
                    const game_core::Board& board
            ) const override {
                opponent.registerHit(coordinate);
                opponent.finishHunt(board);
            }
        };

        /** @brief A shot that was never taken, off the board or at a resolved cell.
         *  It costs nothing: the turn stays and any armed bonus is still waiting. */
        class RejectedBehaviour final : public AttackOutcomeBehaviour {
        public:
            [[nodiscard]] bool keepsTurn() const override {
                return true;
            }

            [[nodiscard]] bool isShotSpent() const override {
                return false;
            }

            [[nodiscard]] bool grantsSkill() const override {
                return false;
            }

            [[nodiscard]] MatchEventKind eventKind() const override {
                return MatchEventKind::ShotRejected;
            }

            void updateHunt(
                    AiOpponent&,
                    game_core::Coordinate,
                    const game_core::Board&
            ) const override {
                // Nothing happened, so the hunt is unchanged.
            }
        };

        const MissBehaviour MISS_BEHAVIOUR;
        const HitBehaviour HIT_BEHAVIOUR;
        const SunkBehaviour SUNK_BEHAVIOUR;
        const RejectedBehaviour REJECTED_BEHAVIOUR;

        /** @brief Behaviours in AttackOutcome's declaration order, indexed by the enum. */
        const std::array<const AttackOutcomeBehaviour*, 5> BEHAVIOURS_BY_OUTCOME{
                &MISS_BEHAVIOUR,     // Miss
                &HIT_BEHAVIOUR,      // Hit
                &SUNK_BEHAVIOUR,     // Sunk
                &REJECTED_BEHAVIOUR, // AlreadyAttacked
                &REJECTED_BEHAVIOUR  // OutOfBounds
        };
    } // namespace

    const AttackOutcomeBehaviour& behaviourFor(game_core::AttackOutcome outcome) {
        return *BEHAVIOURS_BY_OUTCOME.at(static_cast<std::size_t>(outcome));
    }
} // namespace cpp_warships::game_flow
