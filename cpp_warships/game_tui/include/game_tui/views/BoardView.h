#pragma once

#include <optional>
#include <unordered_set>

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/box.hpp>
#include <ftxui/screen/color.hpp>

#include <game_core/Board.h>
#include <game_core/Coordinate.h>
#include <game_core/Outcomes.h>
#include <game_tui/Theme.h>

namespace cpp_warships::game_tui {
    /** @brief What is drawn on top of a board: where the cursor rests and which cells are marked.
     */
    struct BoardOverlay {
        std::optional<game_core::Coordinate> cursor;
        std::unordered_set<game_core::Coordinate> marked;
        ftxui::Color markColor;
    };

    /** @brief Draws a board as a labelled grid and remembers where that grid landed.
     *  The remembered geometry is what turns a mouse position back into a cell. */
    class BoardView {
    public:
        [[nodiscard]] ftxui::Element render(
                const game_core::Board& board,
                game_core::Visibility visibility,
                const Theme& theme,
                const BoardOverlay& overlay
        );

        /** @brief The cell drawn at a screen position, or nullopt when that is off the grid. */
        [[nodiscard]] std::optional<game_core::Coordinate> cellAt(int screenX, int screenY) const;

    private:
        ftxui::Box gridBox_;
        int boardWidth_ = 0;
        int boardHeight_ = 0;
    };
} // namespace cpp_warships::game_tui
