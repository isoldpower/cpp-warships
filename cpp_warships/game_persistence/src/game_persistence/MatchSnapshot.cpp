#include <game_persistence/MatchSnapshot.h>

#include <utility>

namespace cpp_warships::game_persistence {

    MatchSnapshot::MatchSnapshot(
            game_core::MatchSettings settings,
            game_core::Board playerBoard,
            game_core::Board computerBoard,
            std::deque<game_flow::SkillKind> bankedSkills,
            int roundNumber,
            game_flow::MatchPhase phase,
            game_flow::Participant currentTurn,
            bool isDoubleDamageArmed
    )
        : settings_(std::move(settings))
        , playerBoard_(std::move(playerBoard))
        , computerBoard_(std::move(computerBoard))
        , bankedSkills_(std::move(bankedSkills))
        , roundNumber_(roundNumber)
        , phase_(phase)
        , currentTurn_(currentTurn)
        , isDoubleDamageArmed_(isDoubleDamageArmed) {}

    MatchSnapshot MatchSnapshot::capture(const game_flow::Match& match) {
        return MatchSnapshot{
                match.settings(),
                match.playerBoard(),
                match.computerBoard(),
                match.skills().pending(),
                match.roundNumber(),
                match.phase(),
                match.currentTurn(),
                match.isDoubleDamageArmed()
        };
    }

    game_flow::Match MatchSnapshot::restore(game_flow::RandomEngine& randomEngine) const {
        game_flow::MatchRestoreState state{
                .playerBoard = playerBoard_,
                .computerBoard = computerBoard_,
                .bankedSkills = bankedSkills_,
                .roundNumber = roundNumber_,
                .phase = phase_,
                .currentTurn = currentTurn_,
                .isDoubleDamageArmed = isDoubleDamageArmed_
        };

        return game_flow::Match{settings_, randomEngine, std::move(state)};
    }

    const game_core::MatchSettings& MatchSnapshot::settings() const noexcept {
        return settings_;
    }

    const game_core::Board& MatchSnapshot::playerBoard() const noexcept {
        return playerBoard_;
    }

    const game_core::Board& MatchSnapshot::computerBoard() const noexcept {
        return computerBoard_;
    }

    const std::deque<game_flow::SkillKind>& MatchSnapshot::bankedSkills() const noexcept {
        return bankedSkills_;
    }

    int MatchSnapshot::roundNumber() const noexcept {
        return roundNumber_;
    }

    game_flow::MatchPhase MatchSnapshot::phase() const noexcept {
        return phase_;
    }

    game_flow::Participant MatchSnapshot::currentTurn() const noexcept {
        return currentTurn_;
    }

    bool MatchSnapshot::isDoubleDamageArmed() const noexcept {
        return isDoubleDamageArmed_;
    }
} // namespace cpp_warships::game_persistence
