#pragma once

#include <optional>
#include <vector>

#include <game_core/Coordinate.h>
#include <game_flow/Participant.h>
#include <game_flow/SkillKind.h>

namespace cpp_warships::game_flow {
    /** @brief Something that happened during a match, in the order it happened. */
    enum class MatchEventKind {
        ShotMissed,
        ShipDamaged,
        ShipSunk,
        ShotRejected,
        SkillGranted,
        DoubleDamageArmed,
        AreaScanned,
        RoundWon,
        MatchLost,
        TurnPassed,
    };

    /** @brief One entry in a match's history.
     *  The interface renders these and animations replay them; the match draws nothing. */
    struct MatchEvent {
        MatchEventKind kind;
        Participant actor = Participant::Player;
        std::optional<game_core::Coordinate> coordinate = std::nullopt;
        std::optional<SkillKind> skill = std::nullopt;
        /** @brief For AreaScanned: whether the scanned area held a ship. */
        bool scanFoundShip = false;
    };

    using MatchEventLog = std::vector<MatchEvent>;
} // namespace cpp_warships::game_flow
