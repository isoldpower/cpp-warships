#include <game_flow/SkillManager.h>

namespace cpp_warships::game_flow {

    SkillManager::SkillManager(RandomEngine& randomEngine)
        : randomEngine_(randomEngine) {}

    void SkillManager::grantOpeningHand() {
        bank_.grantAllShuffled(randomEngine_);
    }

    SkillKind SkillManager::grantRandom() {
        return bank_.grantRandom(randomEngine_);
    }

    const SkillQueue& SkillManager::bank() const noexcept {
        return bank_;
    }

    bool SkillManager::nextNeedsTarget() const {
        const std::optional<SkillKind> pending = bank_.next();
        return pending.has_value() && behaviourFor(*pending).needsTarget();
    }

    bool SkillManager::applyNext(
            SkillContext& context,
            std::optional<game_core::Coordinate> target
    ) {
        const std::optional<SkillKind> pending = bank_.next();
        bool isApplied = false;

        if (pending.has_value()) {
            const SkillBehaviour& behaviour = behaviourFor(*pending);
            if (!behaviour.needsTarget() || target.has_value()) {
                bank_.consume();
                behaviour.apply(context, target);
                isApplied = true;
            }
        }

        return isApplied;
    }
} // namespace cpp_warships::game_flow
