#pragma once

namespace cpp_warships::game_flow {
    /** @brief How much damage the next shot deals, including any armed bonus.
     *  A bonus survives rejected shots and is spent by the first shot actually taken. */
    class ShotStrength {
    public:
        explicit ShotStrength(int baseDamage);

        [[nodiscard]] int baseDamage() const noexcept;

        /** @brief Damage the next shot would deal. */
        [[nodiscard]] int nextShotDamage() const noexcept;

        [[nodiscard]] bool isDoubleDamageArmed() const noexcept;

        /** @brief Arms the next shot to deal twice the base damage. */
        void armDoubleDamage() noexcept;

        /** @brief Consumes any armed bonus, called once a shot has actually been taken. */
        void spend() noexcept;

    private:
        int baseDamage_;
        bool isDoubleDamageArmed_;
    };
} // namespace cpp_warships::game_flow
