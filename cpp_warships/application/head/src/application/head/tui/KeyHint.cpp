#include <application/head/common/Theme.h>
#include <application/head/tui/FtxuiPalette.h>
#include <application/head/tui/KeyHint.h>
#include <application/head/tui/ScrollPanel.h>

#include <cstddef>
#include <map>
#include <utility>

namespace cpp_warships::head::tui {
    namespace {
        /** @brief How far a legend is held off the edges of its container, in columns. */
        constexpr int LEGEND_PADDING = 2;

        /** @brief Shown under the keys when the terminal hears the character a key typed rather
         * than the key itself, so a shortcut only works in a Latin layout such as ABC. */
        constexpr const char* LATIN_LAYOUT_NOTE = "keys need the ABC layout";

        ftxui::Element padding() {
            return ftxui::text(std::string(static_cast<std::size_t>(LEGEND_PADDING), ' '));
        }

        ftxui::Element hintLine(const common::Theme& theme, const KeyHint& hint) {
            return ftxui::hbox(
                {ftxui::text(" " + hint.key + " ") | color(theme.background) |
                     bgcolor(theme.accent),
                 ftxui::filler(),
                 ftxui::text(hint.description) | color(theme.text)}
            );
        }
    }  // namespace

    std::optional<common::input::Keystroke> keystrokeOfHint(const std::string& key) {
        using common::input::Key;
        static const std::map<std::string, Key> NAMED_KEYS = {
            {"enter", Key::Enter},
            {"esc", Key::Escape},
            {"tab", Key::Tab},
            {"bksp", Key::Backspace},
            {"back", Key::Backspace},
        };

        const auto named = NAMED_KEYS.find(key);
        if (named != NAMED_KEYS.end()) {
            return common::input::Keystroke{.key = named->second};
        }

        if (key.size() == 1) {
            return common::input::Keystroke{.key = Key::Character, .character = key};
        }

        return std::nullopt;
    }

    void KeyHotspots::expect(const std::vector<KeyHint>& hints) {
        hints_ = hints;
        boxes_.assign(hints.size(), ftxui::Box{});
    }

    ftxui::Box& KeyHotspots::boxOf(const std::size_t index) {
        return boxes_.at(index);
    }

    void KeyHotspots::publish(common::input::GridGeometry& geometry) const {
        std::vector<common::input::KeyHotspot> pressable;
        for (std::size_t index = 0; index < hints_.size(); ++index) {
            const std::optional<common::input::Keystroke> stroke =
                keystrokeOfHint(hints_[index].key);
            const ftxui::Box& box = boxes_[index];
            if (!stroke.has_value() || box.x_max < box.x_min || box.y_max < box.y_min) {
                continue;
            }

            pressable.push_back(
                common::input::KeyHotspot{
                    .area =
                        {.left = box.x_min,
                         .top = box.y_min,
                         .right = box.x_max,
                         .bottom = box.y_max},
                    .stroke = *stroke
                }
            );
        }

        geometry.rememberHotspots(std::move(pressable));
    }

    ftxui::Element keyLegend(
        const common::Theme& theme,
        std::vector<KeyHint> hints,
        KeyHotspots& hotspots,
        const bool needsLatinLayout
    ) {
        hints.push_back(KeyHint{.key = "shift arrows", .description = "focus"});
        hints.push_back(KeyHint{.key = "pgup pgdn", .description = "scroll"});
        hotspots.expect(hints);

        std::vector<ftxui::Element> lines;
        for (std::size_t index = 0; index < hints.size(); ++index) {
            lines.push_back(hintLine(theme, hints[index]) | reflectWholeBox(hotspots.boxOf(index)));
        }

        if (needsLatinLayout) {
            lines.push_back(ftxui::text(LATIN_LAYOUT_NOTE) | ftxui::bold | color(theme.danger));
        }

        return ftxui::hbox({padding(), ftxui::vbox(std::move(lines)) | ftxui::flex, padding()});
    }
}  // namespace cpp_warships::head::tui
