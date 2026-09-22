#pragma once

#include <game_tui/Intent.h>

namespace cpp_warships::game_tui {
    /** @brief Asks the application to begin a new match on a board of the given size. */
    class StartMatchIntent final : public Intent {
    public:
        explicit StartMatchIntent(int boardSize);

        void applyTo(Application& application) const override;

    private:
        int boardSize_;
    };
} // namespace cpp_warships::game_tui
