#pragma once

namespace cpp_warships::game_tui {
    /** @brief The screens the application can be showing. */
    enum class ScreenKind {
        Menu,
        Placement,
        Battle,
        GameOver,
    };
} // namespace cpp_warships::game_tui
