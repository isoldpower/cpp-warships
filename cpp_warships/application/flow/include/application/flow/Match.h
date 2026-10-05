#pragma once

#include <application/core/Board.h>
#include <application/core/Coordinate.h>
#include <application/core/MatchSettings.h>
#include <application/core/Outcomes.h>
#include <application/flow/AiOpponent.h>
#include <application/flow/AttackOutcomeBehaviour.h>
#include <application/flow/EventLog.h>
#include <application/flow/MatchPhase.h>
#include <application/flow/PlacementPlan.h>
#include <application/flow/RandomEngine.h>
#include <application/flow/ShotStrength.h>
#include <application/flow/SkillBehaviour.h>
#include <application/flow/SkillManager.h>
#include <application/flow/TurnOrder.h>

#include <deque>
#include <optional>

namespace cpp_warships::flow {
    struct MatchEvent;
}

namespace cpp_warships::flow {
    /** @brief Everything a match needs to carry on from where a save left off. */
    struct MatchRestoreState {
        core::Board playerBoard;
        core::Board computerBoard;
        std::deque<SkillKind> bankedSkills{};
        int roundNumber = 1;
        MatchPhase phase = MatchPhase::Placement;
        Participant currentTurn = Participant::Player;
        bool isDoubleDamageArmed = false;
        AiMemory opponentMemory{};
    };

    /** @brief A game in progress: two boards, whose turn it is and what has happened.
     * Decides everything and draws nothing, reporting events for the interface to render. */
    class Match : private SkillContext {
    public:
        Match(core::MatchSettings settings, RandomEngine& randomEngine);

        /** @brief Resumes a match from @p state rather than starting a fresh one. */
        Match(core::MatchSettings settings, RandomEngine& randomEngine, MatchRestoreState state);

        [[nodiscard]] const core::MatchSettings& settings() const noexcept;
        [[nodiscard]] MatchPhase phase() const noexcept;
        [[nodiscard]] int roundNumber() const noexcept;

        [[nodiscard]] const core::Board& playerBoard() const noexcept;
        [[nodiscard]] const core::Board& computerBoard() const noexcept;

        /** @brief The player's board, writable so the placement screen can lay out ships. */
        [[nodiscard]] core::Board& editablePlayerBoard() noexcept;
        [[nodiscard]] PlacementPlan playerPlacementPlan() const;

        /** @brief Discards the player's layout and lays the fleet out at random. */
        bool shufflePlayerFleet();

        /** @brief Leaves placement and starts the battle, laying out the computer fleet.
         * @return false when the player's fleet is not fully placed. */
        bool beginBattle();

        [[nodiscard]] bool isPlayerTurn() const noexcept;
        [[nodiscard]] Participant currentTurn() const noexcept;
        [[nodiscard]] bool isDoubleDamageArmed() const noexcept;

        /** @brief What the computer has learned, so a save can carry it. */
        [[nodiscard]] AiMemory opponentMemory() const;

        /** @brief Fires at the computer's board on the player's behalf. A hit keeps the turn,
         * a miss passes it, and a rejected shot costs nothing. */
        core::AttackOutcome fireAt(core::Coordinate coordinate);

        /** @brief Plays the computer's shots until it misses or the match ends. */
        void runComputerTurn();

        /** @brief Hands play on once the player's turn is over, letting the enemy answer. */
        void concludeTurn();

        [[nodiscard]] const SkillQueue& skills() const noexcept;

        /** @brief Whether the next banked skill needs a target cell from the player. */
        [[nodiscard]] bool nextSkillNeedsTarget() const;

        /** @brief Applies the next banked skill; @p scanTarget is needed only by Scanner.
         * @return false when nothing is banked, or a needed target is missing. */
        bool applyNextSkill(std::optional<core::Coordinate> scanTarget = std::nullopt);

        /** @brief Returns everything that happened since the last call and clears the log. */
        MatchEventLog drainEvents();

    private:
        [[nodiscard]] const core::Board& enemyBoard() const override;
        [[nodiscard]] RandomEngine& randomEngine() override;
        void strikeEnemyCell(core::Coordinate coordinate) override;
        void armDoubleDamage() override;
        void recordSkillEvent(const MatchEvent& event) override;

        /** @brief Fires a single computer shot at the player's board.
         * @return whether the computer still holds the turn and should fire again. */
        bool takeComputerShot();

        /** @brief Settles a shot of the player's: records it and banks any skill it earned. */
        void recordPlayerShot(core::AttackOutcome outcome, core::Coordinate coordinate);

        /** @brief Hands the turn over, unless @p outcome earned the player another shot. */
        void passTurnUnlessKept(core::AttackOutcome outcome);

        /** @brief Replaces the enemy fleet for a fresh round. Our own board carries over,
         * so what the enemy knows of it carries over too. */
        void startNextRound();
        void concludeAsLoss();

        core::MatchSettings settings_;
        RandomEngine& randomEngine_;
        core::Board playerBoard_;
        core::Board computerBoard_;
        AiOpponent aiOpponent_;
        SkillManager skillManager_;
        ShotStrength shotStrength_;
        TurnOrder turnOrder_;
        EventLog events_;
        MatchPhase phase_;
        int roundNumber_;
    };
}  // namespace cpp_warships::flow
