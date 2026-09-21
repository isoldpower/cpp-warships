#pragma once

#include <optional>

#include <game_core/Coordinate.h>
#include <game_flow/RandomEngine.h>
#include <game_flow/SkillBehaviour.h>
#include <game_flow/SkillKind.h>
#include <game_flow/SkillQueue.h>

namespace cpp_warships::game_flow {
    /** @brief Owns a player's banked skills and applies them.
     *  The match asks for a skill to be used; which skill and what it does live here. */
    class SkillManager {
    public:
        explicit SkillManager(RandomEngine& randomEngine);

        /** @brief Banks the opening hand of one of every skill, in random order. */
        void grantOpeningHand();

        /** @brief Banks one randomly chosen skill, as awarded for sinking a ship. */
        SkillKind grantRandom();

        [[nodiscard]] const SkillQueue& bank() const noexcept;

        /** @brief Whether the next banked skill needs a target cell before it can be applied. */
        [[nodiscard]] bool nextNeedsTarget() const;

        /** @brief Applies and consumes the next banked skill.
         *  @return false when nothing is banked, or a needed target is missing. */
        bool applyNext(SkillContext& context, std::optional<game_core::Coordinate> target);

    private:
        RandomEngine& randomEngine_;
        SkillQueue bank_;
    };
} // namespace cpp_warships::game_flow
