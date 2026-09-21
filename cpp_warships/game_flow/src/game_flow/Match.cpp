#include <game_flow/Match.h>

#include <utility>

namespace cpp_warships::game_flow {
    Match::Match(game_core::MatchSettings settings, RandomEngine& randomEngine)
        : settings_(std::move(settings))
        , randomEngine_(randomEngine)
        , playerBoard_(settings_.boardSize(), settings_.boardSize())
        , computerBoard_(settings_.boardSize(), settings_.boardSize())
        , aiOpponent_(randomEngine)
        , skillManager_(randomEngine)
        , shotStrength_(settings_.baseDamage())
        , turnOrder_(Participant::Player)
        , phase_(MatchPhase::Placement)
        , roundNumber_(1) {
        skillManager_.grantOpeningHand();
    }

    const game_core::MatchSettings& Match::settings() const noexcept {
        return settings_;
    }

    MatchPhase Match::phase() const noexcept {
        return phase_;
    }

    int Match::roundNumber() const noexcept {
        return roundNumber_;
    }

    const game_core::Board& Match::playerBoard() const noexcept {
        return playerBoard_;
    }

    const game_core::Board& Match::computerBoard() const noexcept {
        return computerBoard_;
    }

    game_core::Board& Match::editablePlayerBoard() noexcept {
        return playerBoard_;
    }

    PlacementPlan Match::playerPlacementPlan() const {
        return PlacementPlan{settings_.fleet(), playerBoard_};
    }

    bool Match::shufflePlayerFleet() {
        return placeFleetRandomly(playerBoard_, settings_.fleet(), randomEngine_);
    }

    bool Match::beginBattle() {
        if (phase_ != MatchPhase::Placement || !playerPlacementPlan().isComplete()) {
            return false;
        } else if (!placeFleetRandomly(computerBoard_, settings_.fleet(), randomEngine_)) {
            return false;
        }

        phase_ = MatchPhase::Battle;
        turnOrder_.giveTo(Participant::Player);
        return true;
    }

    bool Match::isPlayerTurn() const noexcept {
        return turnOrder_.isPlayerTurn();
    }

    void Match::startNextRound() {
        events_.record({
                .kind = MatchEventKind::RoundWon,
                .actor = Participant::Player,
        });

        aiOpponent_.reset();
        placeFleetRandomly(computerBoard_, settings_.fleet(), randomEngine_);
        ++roundNumber_;
        turnOrder_.giveTo(Participant::Player);
    }

    void Match::concludeAsLoss() {
        events_.record({.kind = MatchEventKind::MatchLost, .actor = Participant::Computer});
        phase_ = MatchPhase::Finished;
    }

    void Match::applyShotOutcome(
            game_core::AttackOutcome outcome,
            game_core::Coordinate coordinate,
            Participant actor
    ) {
        const AttackOutcomeBehaviour& behaviour = behaviourFor(outcome);
        events_.record(
            {.kind = behaviour.eventKind(),
                .actor = actor,
                .coordinate = coordinate}
        );

        if (behaviour.grantsSkill() && actor == Participant::Player) {
            const SkillKind granted = skillManager_.grantRandom();
            events_.record(
                    {.kind = MatchEventKind::SkillGranted,
                        .actor = actor,
                        .skill = granted}
            );
        }

        if (!behaviour.keepsTurn()) {
            turnOrder_.pass();
            events_.record(
                {.kind = MatchEventKind::TurnPassed,
                    .actor = turnOrder_.current()}
            );
        }
    }

    game_core::AttackOutcome Match::fireAt(game_core::Coordinate coordinate) {
        if (phase_ != MatchPhase::Battle || !turnOrder_.isPlayerTurn()) {
            return game_core::AttackOutcome::AlreadyAttacked;
        }

        const game_core::AttackOutcome outcome =
                computerBoard_.attack(coordinate, shotStrength_.nextShotDamage());
        if (behaviourFor(outcome).isShotSpent()) {
            shotStrength_.spend();
        }

        applyShotOutcome(outcome, coordinate, Participant::Player);
        if (computerBoard_.allShipsSunk()) {
            startNextRound();
        }

        return outcome;
    }

    bool Match::takeComputerShot() {
        const std::optional<game_core::Coordinate> targetCell =
                aiOpponent_.chooseTarget(playerBoard_);
        bool keepsTurn = false;

        if (targetCell.has_value()) {
            const game_core::AttackOutcome outcome =
                    playerBoard_.attack(*targetCell, settings_.baseDamage());
            aiOpponent_.recordOutcome(*targetCell, outcome, playerBoard_);

            const AttackOutcomeBehaviour& behaviour = behaviourFor(outcome);
            keepsTurn = behaviour.keepsTurn() && behaviour.isShotSpent();
            events_.record(
                    {.kind = behaviour.eventKind(),
                     .actor = Participant::Computer,
                     .coordinate = *targetCell}
            );
        }

        return keepsTurn;
    }

    void Match::runComputerTurn() {
        if (phase_ != MatchPhase::Battle || turnOrder_.isPlayerTurn()) {
            return;
        }

        bool keepsTurn = true;
        bool isPlayerFleetSunk = false;

        while (keepsTurn && !isPlayerFleetSunk) {
            keepsTurn = takeComputerShot();
            isPlayerFleetSunk = playerBoard_.allShipsSunk();
        }

        if (isPlayerFleetSunk) {
            concludeAsLoss();
        } else {
            turnOrder_.giveTo(Participant::Player);
            events_.record(
                {.kind = MatchEventKind::TurnPassed,
                    .actor = Participant::Player}
            );
        }
    }

    const SkillQueue& Match::skills() const noexcept {
        return skillManager_.bank();
    }

    bool Match::nextSkillNeedsTarget() const {
        return skillManager_.nextNeedsTarget();
    }

    bool Match::applyNextSkill(std::optional<game_core::Coordinate> scanTarget) {
        if (phase_ != MatchPhase::Battle || !turnOrder_.isPlayerTurn()) {
            return false;
        }

        return skillManager_.applyNext(*this, scanTarget);
    }

    const game_core::Board& Match::enemyBoard() const {
        return computerBoard_;
    }

    RandomEngine& Match::randomEngine() {
        return randomEngine_;
    }

    void Match::armDoubleDamage() {
        shotStrength_.armDoubleDamage();
    }

    void Match::strikeEnemyCell(game_core::Coordinate coordinate) {
        const game_core::AttackOutcome outcome =
                computerBoard_.attack(coordinate, shotStrength_.baseDamage());
        applyShotOutcome(outcome, coordinate, Participant::Player);

        if (computerBoard_.allShipsSunk()) {
            startNextRound();
        }
    }

    void Match::recordSkillEvent(const MatchEvent& event) {
        events_.record(event);
    }

    MatchEventLog Match::drainEvents() {
        return events_.drain();
    }
} // namespace cpp_warships::game_flow
