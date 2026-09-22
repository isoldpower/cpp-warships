#include <game_tui/views/KeyHint.h>

namespace cpp_warships::game_tui {
    ftxui::Element keyHint(
            const Theme& theme,
            const std::string& key,
            const std::string& description
    ) {
        return ftxui::hbox(
                {ftxui::text(" " + key + " ") | ftxui::color(theme.background) |
                         ftxui::bgcolor(theme.accent),
                 ftxui::text("  " + description) | ftxui::color(theme.text)}
        );
    }
} // namespace cpp_warships::game_tui
