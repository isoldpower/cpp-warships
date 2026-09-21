#pragma once

namespace cpp_warships::game_core {
    inline constexpr int DEFAULT_SEGMENT_HEALTH = 2;

    /** @brief One cell of a ship, tracking its own hit points. */
    class Segment {
    public:
        explicit Segment(int maximumHealth) noexcept;
        Segment(int maximumHealth, int health) noexcept;

        /** @brief Applies damage, clamping at zero. */
        void takeDamage(int amount) noexcept;

        [[nodiscard]] int health() const noexcept;
        [[nodiscard]] int maximumHealth() const noexcept;
        [[nodiscard]] bool isDestroyed() const noexcept;

    private:
        int maximumHealth_;
        int health_;
    };
} // namespace cpp_warships::game_core
