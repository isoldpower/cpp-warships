#include <game_flow/SkillQueue.h>

#include <algorithm>
#include <utility>

namespace cpp_warships::game_flow {

    SkillQueue::SkillQueue(std::deque<SkillKind> pending)
        : pending_(std::move(pending)) {}

    SkillKind SkillQueue::grantRandom(RandomEngine& randomEngine) {
        std::uniform_int_distribution<std::size_t> distribution{0, ALL_SKILL_KINDS.size() - 1};
        const SkillKind granted = ALL_SKILL_KINDS[distribution(randomEngine)];
        grant(granted);
        return granted;
    }

    void SkillQueue::grantAllShuffled(RandomEngine& randomEngine) {
        std::array<SkillKind, ALL_SKILL_KINDS.size()> shuffled = ALL_SKILL_KINDS;
        std::shuffle(shuffled.begin(), shuffled.end(), randomEngine);
        for (const SkillKind skill : shuffled) {
            grant(skill);
        }
    }

    void SkillQueue::grant(SkillKind skill) {
        pending_.push_back(skill);
    }

    std::optional<SkillKind> SkillQueue::next() const {
        if (pending_.empty()) {
            return std::nullopt;
        }
        return pending_.front();
    }

    std::optional<SkillKind> SkillQueue::consume() {
        if (pending_.empty()) {
            return std::nullopt;
        }

        const SkillKind skill = pending_.front();
        pending_.pop_front();
        return skill;
    }

    const std::deque<SkillKind>& SkillQueue::pending() const noexcept {
        return pending_;
    }

    bool SkillQueue::isEmpty() const noexcept {
        return pending_.empty();
    }
} // namespace cpp_warships::game_flow
