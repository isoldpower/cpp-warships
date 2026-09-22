#pragma once

#include <functional>

#include <game_flow/Match.h>

namespace cpp_warships::game_tui {
    /** @brief Read-only reach into the match in play, so a screen can draw it but never change it.
     *  Changing the match is an intent's job, and this const reference is what enforces that. */
    using MatchQuery = std::function<const game_flow::Match&()>;
} // namespace cpp_warships::game_tui
