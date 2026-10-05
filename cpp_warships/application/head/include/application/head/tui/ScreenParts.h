#pragma once

#include <application/head/common/Theme.h>

#include <ftxui/dom/elements.hpp>
#include <string>

namespace cpp_warships::head::tui {
    /** @brief A rule between two panels of a screen. */
    [[nodiscard]] ftxui::Element divider(const common::Theme& theme);

    /** @brief The heading of a panel that lists things, such as SKILLS or KEYS. */
    [[nodiscard]] ftxui::Element sectionHeading(
        const common::Theme& theme,
        const std::string& title
    );
}  // namespace cpp_warships::head::tui
