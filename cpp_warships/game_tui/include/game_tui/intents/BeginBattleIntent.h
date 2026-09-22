#pragma once

#include <game_tui/Intent.h>

namespace cpp_warships::game_tui {
    /** @brief Asks the application to leave placement and open fire. */
    class BeginBattleIntent final : public Intent {
    public:
        void applyTo(Application& application) const override;
    };
} // namespace cpp_warships::game_tui
