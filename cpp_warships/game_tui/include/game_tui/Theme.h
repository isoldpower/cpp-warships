#pragma once

#include <string>
#include <vector>

#include <ftxui/screen/color.hpp>

namespace cpp_warships::game_tui {
    /** @brief Every colour the interface may use, named by role rather than by hue.
     *  Screens read these slots and never name a colour themselves. */
    struct Theme {
        std::string name;

        ftxui::Color background;
        ftxui::Color surface;
        ftxui::Color border;
        ftxui::Color text;
        ftxui::Color textMuted;
        ftxui::Color accent;

        ftxui::Color water;
        ftxui::Color ship;
        ftxui::Color hit;
        ftxui::Color sunk;
        ftxui::Color miss;
        ftxui::Color cursor;

        ftxui::Color danger;
        ftxui::Color success;
    };

    /** @brief The themes the interface can be dressed in, in the order they are offered. */
    [[nodiscard]] const std::vector<Theme>& availableThemes();

    /** @brief The theme used until the player picks another. */
    [[nodiscard]] const Theme& defaultTheme();

    /** @brief The theme called @p name, or the default when there is no such theme. */
    [[nodiscard]] const Theme& themeNamed(const std::string& name);
} // namespace cpp_warships::game_tui
