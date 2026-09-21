#include <game_flow/ShotStrength.h>

namespace cpp_warships::game_flow {
    namespace {
        /** @brief What arming double damage multiplies the base damage by. */
        constexpr int DOUBLE_DAMAGE_MULTIPLIER = 2;
    } // namespace

    ShotStrength::ShotStrength(int baseDamage)
        : ShotStrength(baseDamage, false) {}

    ShotStrength::ShotStrength(int baseDamage, bool isDoubleDamageArmed)
        : baseDamage_(baseDamage)
        , isDoubleDamageArmed_(isDoubleDamageArmed) {}

    int ShotStrength::baseDamage() const noexcept {
        return baseDamage_;
    }

    int ShotStrength::nextShotDamage() const noexcept {
        return isDoubleDamageArmed_ ? baseDamage_ * DOUBLE_DAMAGE_MULTIPLIER : baseDamage_;
    }

    bool ShotStrength::isDoubleDamageArmed() const noexcept {
        return isDoubleDamageArmed_;
    }

    void ShotStrength::armDoubleDamage() noexcept {
        isDoubleDamageArmed_ = true;
    }

    void ShotStrength::spend() noexcept {
        isDoubleDamageArmed_ = false;
    }
} // namespace cpp_warships::game_flow
