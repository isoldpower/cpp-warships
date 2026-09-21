#pragma once

#include <array>

namespace cpp_warships::game_flow {
    /** @brief The skills a player can be granted for sinking a ship. */
    enum class SkillKind {
        Scanner,
        DoubleDamage,
        RandomStrike,
    };

    inline constexpr std::array<SkillKind, 3> ALL_SKILL_KINDS{
            SkillKind::Scanner,
            SkillKind::DoubleDamage,
            SkillKind::RandomStrike,
    };
} // namespace cpp_warships::game_flow
