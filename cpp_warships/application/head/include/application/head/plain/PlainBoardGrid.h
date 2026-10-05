#pragma once

#include <application/core/Coordinate.h>
#include <application/core/Outcomes.h>

#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

namespace cpp_warships::core {
    class Board;
}

namespace cpp_warships::head::plain {
    /** @brief What is drawn on top of a plain board: where the cursor rests, which cells are
     * spoken for, and the letter those cells are marked with. */
    struct PlainBoardOverlay {
        std::optional<core::Coordinate> cursor{};
        std::unordered_set<core::Coordinate> marked{};
        char markGlyph = '+';
    };

    /** @brief A board ruled out in ASCII, the way the console game used to draw it. */
    [[nodiscard]] std::vector<std::string> plainBoardLines(
        const core::Board& board,
        core::Visibility visibility,
        const PlainBoardOverlay& overlay
    );

    /** @brief What each cell state is written as, so a legend can be printed
     * beside a board. */
    [[nodiscard]] std::string plainBoardLegend();
}  // namespace cpp_warships::head::plain
