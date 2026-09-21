#include <game_core/Segment.h>

#include <algorithm>

namespace cpp_warships::game_core {

    Segment::Segment(int maximumHealth) noexcept
        : maximumHealth_(maximumHealth)
        , health_(maximumHealth) {}

    Segment::Segment(int maximumHealth, int health) noexcept
        : maximumHealth_(maximumHealth)
        , health_(std::clamp(health, 0, maximumHealth)) {}

    void Segment::takeDamage(int amount) noexcept {
        health_ = std::max(0, health_ - amount);
    }

    int Segment::health() const noexcept {
        return health_;
    }

    int Segment::maximumHealth() const noexcept {
        return maximumHealth_;
    }

    bool Segment::isDestroyed() const noexcept {
        return health_ == 0;
    }
} // namespace cpp_warships::game_core
