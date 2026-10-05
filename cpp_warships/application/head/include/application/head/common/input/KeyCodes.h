#pragma once

#include <application/head/common/input/Keystroke.h>

#include <optional>
#include <string_view>

namespace cpp_warships::head::common::input {
    /** @brief Asks the terminal, through the kitty keyboard protocol, to report every key as a
     *  code that also names the key it sits on in the US layout, so shortcuts follow the
     *  physical key in any language. A terminal without the protocol ignores it. */
    inline constexpr std::string_view KEY_CODES_REQUEST = "\x1b[>12u";

    /** @brief Asks whether the terminal speaks the protocol; only one that does answers. */
    inline constexpr std::string_view KEY_CODES_QUERY = "\x1b[?u";

    /** @brief Hands the terminal back the way it reported keys before the request. */
    inline constexpr std::string_view KEY_CODES_RELEASE = "\x1b[<u";

    /** @brief Whether @p sequence is the terminal's answer to KEY_CODES_QUERY, which proves it
     * reports keys by position. */
    [[nodiscard]] bool isKeyCodesAnswer(std::string_view sequence);

    /** @brief What a kitty keyboard protocol report in @p sequence stands for, or nothing when
     * @p sequence is not such a report; a key the game has no use for reads as Key::None. */
    [[nodiscard]] std::optional<Keystroke> keystrokeOfKeyCode(std::string_view sequence);
}  // namespace cpp_warships::head::common::input
