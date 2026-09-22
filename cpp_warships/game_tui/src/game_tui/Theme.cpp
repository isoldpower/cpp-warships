#include <game_tui/Theme.h>

#include <algorithm>

namespace cpp_warships::game_tui {
    namespace {
        Theme makeMidnightTheme() {
            return Theme{
                    .name = "midnight",
                    .background = ftxui::Color::RGB(16, 18, 28),
                    .surface = ftxui::Color::RGB(28, 32, 48),
                    .border = ftxui::Color::RGB(64, 72, 104),
                    .text = ftxui::Color::RGB(226, 232, 248),
                    .textMuted = ftxui::Color::RGB(136, 146, 178),
                    .accent = ftxui::Color::RGB(126, 190, 255),
                    .water = ftxui::Color::RGB(52, 62, 92),
                    .ship = ftxui::Color::RGB(168, 180, 208),
                    .hit = ftxui::Color::RGB(246, 170, 86),
                    .sunk = ftxui::Color::RGB(232, 92, 92),
                    .miss = ftxui::Color::RGB(96, 106, 138),
                    .cursor = ftxui::Color::RGB(126, 190, 255),
                    .danger = ftxui::Color::RGB(232, 92, 92),
                    .success = ftxui::Color::RGB(126, 210, 150)
            };
        }

        Theme makeHarbourTheme() {
            return Theme{
                    .name = "harbour",
                    .background = ftxui::Color::RGB(246, 244, 238),
                    .surface = ftxui::Color::RGB(232, 228, 218),
                    .border = ftxui::Color::RGB(176, 168, 152),
                    .text = ftxui::Color::RGB(38, 40, 44),
                    .textMuted = ftxui::Color::RGB(118, 116, 112),
                    .accent = ftxui::Color::RGB(28, 104, 168),
                    .water = ftxui::Color::RGB(202, 212, 222),
                    .ship = ftxui::Color::RGB(72, 80, 92),
                    .hit = ftxui::Color::RGB(198, 118, 30),
                    .sunk = ftxui::Color::RGB(176, 48, 48),
                    .miss = ftxui::Color::RGB(150, 156, 164),
                    .cursor = ftxui::Color::RGB(28, 104, 168),
                    .danger = ftxui::Color::RGB(176, 48, 48),
                    .success = ftxui::Color::RGB(46, 134, 82)
            };
        }
    } // namespace

    const std::vector<Theme>& availableThemes() {
        static const std::vector<Theme> themes{makeMidnightTheme(), makeHarbourTheme()};
        return themes;
    }

    const Theme& defaultTheme() {
        return availableThemes().front();
    }

    const Theme& themeNamed(const std::string& name) {
        const auto hasName = [&name](const Theme& theme) {
            return theme.name == name;
        };

        const auto& themes = availableThemes();
        const auto found = std::find_if(
            themes.begin(),
            themes.end(),
            hasName
        );

        return found == themes.end() ? defaultTheme() : *found;
    }
} // namespace cpp_warships::game_tui
