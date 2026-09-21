#pragma once

#include <optional>

#include <game_core/Board.h>
#include <game_core/Coordinate.h>
#include <game_core/MatchSettings.h>
#include <game_core/Outcomes.h>
#include <game_flow/AiOpponent.h>
#include <game_flow/AttackOutcomeBehaviour.h>
#include <game_flow/EventLog.h>
#include <game_flow/MatchEvent.h>
#include <game_flow/PlacementPlan.h>
#include <game_flow/RandomEngine.h>
#include <game_flow/ShotStrength.h>
#include <game_flow/SkillBehaviour.h>
#include <game_flow/SkillManager.h>
#include <game_flow/TurnOrder.h>

namespace cpp_warships::game_flow {
    /** @brief Which part of a match is being played. */
    enum class MatchPhase {
        Placement,
        Battle,
        Finished,
    };

    /** @brief A game in progress: two boards, whose turn it is and what has happened.
     *  Decides everything and draws nothing, reporting events for the interface to render. */
    class Match : private SkillContext {
    public:
        Match(game_core::MatchSettings settings, RandomEngine& randomEngine);

        [[nodiscard]] const game_core::MatchSettings& settings() const noexcept;
        [[nodiscard]] MatchPhase phase() const noexcept;
        [[nodiscard]] int roundNumber() const noexcept;

        [[nodiscard]] const game_core::Board& playerBoard() const noexcept;
        [[nodiscard]] const game_core::Board& computerBoard() const noexcept;

        // -- placement phase ------------------------------------------------

        /** @brief The player's board, writable so the placement screen can lay out ships. */
        [[nodiscard]] game_core::Board& editablePlayerBoard() noexcept;
        [[nodiscard]] PlacementPlan playerPlacementPlan() const;

        /** @brief Discards the player's layout and lays the fleet out at random. */
        bool shufflePlayerFleet();

        /** @brief Leaves placement and starts the battle, laying out the computer fleet.
         *  @return false when the player's fleet is not fully placed. */
        bool beginBattle();

        // -- battle phase ---------------------------------------------------

        [[nodiscard]] bool isPlayerTurn() const noexcept;

        /** @brief Fires at the computer's board on the player's behalf.
         *  A hit keeps the turn, a miss passes it, and a rejected shot costs nothing. */
        game_core::AttackOutcome fireAt(game_core::Coordinate coordinate);

        /** @brief Plays the computer's shots until it misses or the match ends. */
        void runComputerTurn();

        // -- skills ---------------------------------------------------------

        [[nodiscard]] const SkillQueue& skills() const noexcept;

        /** @brief Whether the next banked skill needs a target cell from the player. */
        [[nodiscard]] bool nextSkillNeedsTarget() const;

        /** @brief Applies the next banked skill; @p scanTarget is needed only by Scanner.
         *  @return false when nothing is banked, or a needed target is missing. */
        bool applyNextSkill(std::optional<game_core::Coordinate> scanTarget = std::nullopt);

        // -- events ---------------------------------------------------------

        /** @brief Returns everything that happened since the last call and clears the log. */
        MatchEventLog drainEvents();

    private:
        // SkillContext: the primitives a skill is allowed to act through.
        [[nodiscard]] const game_core::Board& enemyBoard() const override;
        [[nodiscard]] RandomEngine& randomEngine() override;
        void strikeEnemyCell(game_core::Coordinate coordinate) override;
        void armDoubleDamage() override;
        void recordSkillEvent(const MatchEvent& event) override;

        /** @brief Fires a single computer shot at the player's board.
         *  @return whether the computer still holds the turn and should fire again. */
        bool takeComputerShot();

        void applyShotOutcome(
                game_core::AttackOutcome outcome,
                game_core::Coordinate coordinate,
                Participant actor
        );
        void startNextRound();
        void concludeAsLoss();

        game_core::MatchSettings settings_;
        RandomEngine& randomEngine_;
        game_core::Board playerBoard_;
        game_core::Board computerBoard_;
        AiOpponent aiOpponent_;
        SkillManager skillManager_;
        ShotStrength shotStrength_;
        TurnOrder turnOrder_;
        EventLog events_;
        MatchPhase phase_;
        int roundNumber_;
    };
} // namespace cpp_warships::game_flow
