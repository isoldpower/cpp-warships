#pragma once

namespace cpp_warships::game_tui {
    /** @brief What the player has chosen on the menu but not yet acted on. */
    struct MenuState {
        int selectedBoardSize = 10;
        int selectedThemeIndex = 0;
    };
} // namespace cpp_warships::game_tui
