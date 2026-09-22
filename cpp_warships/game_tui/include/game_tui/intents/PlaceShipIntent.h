#pragma once

#include <game_core/Coordinate.h>
#include <game_core/Direction.h>
#include <game_tui/Intent.h>

namespace cpp_warships::game_tui {
    /** @brief Asks the application to lay a ship of the given length on the player's board. */
    class PlaceShipIntent final : public Intent {
    public:
        PlaceShipIntent(game_core::Coordinate origin, game_core::Direction direction, int length);

        void applyTo(Application& application) const override;

    private:
        game_core::Coordinate origin_;
        game_core::Direction direction_;
        int length_;
    };
} // namespace cpp_warships::game_tui
