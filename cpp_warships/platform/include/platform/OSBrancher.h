#pragma once

#include <ctime>
#include <optional>
#include <string>

namespace cpp_warships::platform {
    /** @brief Everything the game needs that one operating system spells differently from
     * another. Every branch on the host lives behind these, so nothing else has to. */

    /** @brief Whether standard output is a terminal rather than a pipe or a file. */
    [[nodiscard]] bool isOutputTerminal();

    /** @brief @p moment broken into the fields of local calendar time. */
    [[nodiscard]] std::tm localTimeOf(std::time_t moment);

    /** @brief What the environment says @p name is, or nothing when it says nothing. */
    [[nodiscard]] std::optional<std::string> environmentValue(const char* name);

    /** @brief Where this system keeps a player's own files. */
    [[nodiscard]] std::optional<std::string> homeDirectory();

    /** @brief Whether the host turns a key into its Latin letter in any keyboard layout before
     * the game hears it, as the browser page does. */
    [[nodiscard]] bool hostTranslatesKeyboardLayouts();

    /** @brief Makes the console able to show the glyphs the interface draws with. */
    void prepareConsoleForUnicode();
}  // namespace cpp_warships::platform
