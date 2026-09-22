#pragma once

#include <string>

#include <ftxui/dom/elements.hpp>

#include <game_tui/Theme.h>

namespace cpp_warships::game_tui {
    /** @brief One line of the key legend: the key in a badge, then what it does. */
    [[nodiscard]] ftxui::Element keyHint(
            const Theme& theme,
            const std::string& key,
            const std::string& description
    );
} // namespace cpp_warships::game_tui
