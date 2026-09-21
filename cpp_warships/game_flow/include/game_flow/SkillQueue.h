#pragma once

#include <deque>
#include <optional>

#include <game_flow/RandomEngine.h>
#include <game_flow/SkillKind.h>

namespace cpp_warships::game_flow {
    /** @brief The skills a player has banked, consumed oldest first. */
    class SkillQueue {
    public:
        SkillQueue() = default;
        explicit SkillQueue(std::deque<SkillKind> pending);

        /** @brief Adds one randomly chosen skill, as awarded for sinking a ship. */
        SkillKind grantRandom(RandomEngine& randomEngine);

        /** @brief Banks one of every skill, in random order.
         *  A match opens with the full set; sinking ships tops it up from there. */
        void grantAllShuffled(RandomEngine& randomEngine);
        void grant(SkillKind skill);

        /** @brief The skill that would be applied next, if any. */
        [[nodiscard]] std::optional<SkillKind> next() const;

        /** @brief Removes and returns the next skill, or nullopt when none is banked. */
        std::optional<SkillKind> consume();

        [[nodiscard]] const std::deque<SkillKind>& pending() const noexcept;
        [[nodiscard]] bool isEmpty() const noexcept;

    private:
        std::deque<SkillKind> pending_;
    };
} // namespace cpp_warships::game_flow
