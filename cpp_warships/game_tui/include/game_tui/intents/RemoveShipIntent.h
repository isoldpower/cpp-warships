#pragma once

#include <game_core/Coordinate.h>
#include <game_tui/Intent.h>

namespace cpp_warships::game_tui {
    /** @brief Asks the application to take back the ship covering a cell. */
    class RemoveShipIntent final : public Intent {
    public:
        explicit RemoveShipIntent(game_core::Coordinate coordinate);

        void applyTo(Application& application) const override;

    private:
        game_core::Coordinate coordinate_;
    };
} // namespace cpp_warships::game_tui
