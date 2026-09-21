#pragma once

#include <game_core/FleetComposition.h>
#include <game_core/Segment.h>

namespace cpp_warships::game_core {
    /**
     * @brief Immutable rules a match is played under.
     *
     * Transient combat state such as an armed double-damage shot is match state, not a
     * setting, and lives in game_flow instead.
     */
    class MatchSettings {
    public:
        MatchSettings(
                int boardSize,
                FleetComposition fleet,
                int baseDamage = 1,
                int segmentHealth = DEFAULT_SEGMENT_HEALTH);

        /** @brief Settings for @p boardSize with an automatically scaled fleet. */
        [[nodiscard]] static MatchSettings forBoardSize(int boardSize);

        [[nodiscard]] int boardSize() const noexcept;
        [[nodiscard]] const FleetComposition& fleet() const noexcept;
        [[nodiscard]] int baseDamage() const noexcept;
        [[nodiscard]] int segmentHealth() const noexcept;

    private:
        int boardSize_;
        FleetComposition fleet_;
        int baseDamage_;
        int segmentHealth_;
    };
} // namespace cpp_warships::game_core
