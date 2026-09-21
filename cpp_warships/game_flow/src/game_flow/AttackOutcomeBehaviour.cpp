#include <game_flow/AttackOutcomeBehaviour.h>

#include <unordered_map>

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
            ) const override {}
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
            ) const override {}
        };

        const MissBehaviour MISS_BEHAVIOUR;
        const HitBehaviour HIT_BEHAVIOUR;
        const SunkBehaviour SUNK_BEHAVIOUR;
        const RejectedBehaviour REJECTED_BEHAVIOUR;

        const std::unordered_map<game_core::AttackOutcome, const AttackOutcomeBehaviour*>
                BEHAVIOUR_BY_OUTCOME{
                        {game_core::AttackOutcome::Miss, &MISS_BEHAVIOUR},
                        {game_core::AttackOutcome::Hit, &HIT_BEHAVIOUR},
                        {game_core::AttackOutcome::Sunk, &SUNK_BEHAVIOUR},
                        {game_core::AttackOutcome::AlreadyAttacked, &REJECTED_BEHAVIOUR},
                        {game_core::AttackOutcome::OutOfBounds, &REJECTED_BEHAVIOUR}
                };
    } // namespace

    const AttackOutcomeBehaviour& behaviourFor(game_core::AttackOutcome outcome) {
        return *BEHAVIOUR_BY_OUTCOME.at(outcome);
    }
} // namespace cpp_warships::game_flow
