#pragma once

#include <ftxui/screen/color.hpp>
#include <string>

#include <game_core/Outcomes.h>
#include <game_tui/Theme.h>

namespace cpp_warships::game_tui {
    /** @brief How one board cell is drawn: the glyph inside it and the colour it takes. */
    struct CellAppearance {
        std::string glyph;
        ftxui::Color foreground;
    };

    /** @brief The appearance @p state takes under @p theme. */
    [[nodiscard]] CellAppearance appearanceOf(game_core::CellState state, const Theme& theme);
} // namespace cpp_warships::game_tui
