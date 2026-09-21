#pragma once

#include <deque>

#include <game_core/Board.h>
#include <game_core/MatchSettings.h>
#include <game_flow/Match.h>
#include <game_flow/Participant.h>
#include <game_flow/SkillKind.h>
#include <serialization/ISerializable.h>

namespace cpp_warships::game_persistence {
    inline char MATCH_SNAPSHOT_NAME[] = "MatchSnapshot";

    /** @brief A whole match frozen into plain data, ready to be written out or read back.
     *  Only this layer knows about serialising, which is what keeps the rules free of it. */
    class MatchSnapshot final : public serialization::ISerializable<MATCH_SNAPSHOT_NAME> {
    public:
        MatchSnapshot(
                game_core::MatchSettings settings,
                game_core::Board playerBoard,
                game_core::Board computerBoard,
                std::deque<game_flow::SkillKind> bankedSkills,
                int roundNumber,
                game_flow::MatchPhase phase,
                game_flow::Participant currentTurn,
                bool isDoubleDamageArmed
        );

        /** @brief Captures @p match exactly as it stands. */
        [[nodiscard]] static MatchSnapshot capture(const game_flow::Match& match);

        /** @brief Rebuilds a match from this snapshot, drawing new randomness from @p engine. */
        [[nodiscard]] game_flow::Match restore(game_flow::RandomEngine& randomEngine) const;

        [[nodiscard]] const game_core::MatchSettings& settings() const noexcept;
        [[nodiscard]] const game_core::Board& playerBoard() const noexcept;
        [[nodiscard]] const game_core::Board& computerBoard() const noexcept;
        [[nodiscard]] const std::deque<game_flow::SkillKind>& bankedSkills() const noexcept;
        [[nodiscard]] int roundNumber() const noexcept;
        [[nodiscard]] game_flow::MatchPhase phase() const noexcept;
        [[nodiscard]] game_flow::Participant currentTurn() const noexcept;
        [[nodiscard]] bool isDoubleDamageArmed() const noexcept;

    private:
        game_core::MatchSettings settings_;
        game_core::Board playerBoard_;
        game_core::Board computerBoard_;
        std::deque<game_flow::SkillKind> bankedSkills_;
        int roundNumber_;
        game_flow::MatchPhase phase_;
        game_flow::Participant currentTurn_;
        bool isDoubleDamageArmed_;
    };
} // namespace cpp_warships::game_persistence
