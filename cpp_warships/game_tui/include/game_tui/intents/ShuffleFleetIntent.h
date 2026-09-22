#pragma once

#include <game_tui/Intent.h>

namespace cpp_warships::game_tui {
    /** @brief Asks the application to throw the player's fleet back onto the board at random. */
    class ShuffleFleetIntent final : public Intent {
    public:
        void applyTo(Application& application) const override;
    };
} // namespace cpp_warships::game_tui
