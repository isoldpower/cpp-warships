#pragma once

#include <application/head/common/input/GridGeometry.h>
#include <application/head/common/input/Keystroke.h>

#include <cstddef>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/box.hpp>
#include <optional>
#include <string>
#include <vector>

namespace cpp_warships::head::common {
    struct Theme;
}

namespace cpp_warships::head::tui {
    /** @brief One line of the key legend: the key, and what it does. */
    struct KeyHint {
        std::string key;
        std::string description;
    };

    /** @brief The keystroke a hint's key stands for, when it stands for exactly one; a hint
     * such as "arrows" stands for several and so cannot be pressed by clicking it. */
    [[nodiscard]] std::optional<common::input::Keystroke> keystrokeOfHint(const std::string& key);

    /** @brief Where the hints of a legend landed when it was laid out, so a click on one
     * can press its key. */
    class KeyHotspots {
    public:
        /** @brief Forgets the last legend and makes room to note where @p hints land. */
        void expect(const std::vector<KeyHint>& hints);

        /** @brief The box the hint at @p index will be laid out in. */
        [[nodiscard]] ftxui::Box& boxOf(std::size_t index);

        /** @brief Writes down where every hint that stands for one key landed. */
        void publish(common::input::GridGeometry& geometry) const;

    private:
        std::vector<KeyHint> hints_;
        std::vector<ftxui::Box> boxes_;
    };

    /** @brief A block of key hints, held clear of the edges of whatever contains it, and
     *  closed by the keys that move between panels and scroll them, which work everywhere.
     *  Notes in @p hotspots where each hint landed, and warns when @p needsLatinLayout. */
    [[nodiscard]] ftxui::Element keyLegend(
        const common::Theme& theme,
        std::vector<KeyHint> hints,
        KeyHotspots& hotspots,
        bool needsLatinLayout
    );
}  // namespace cpp_warships::head::tui
