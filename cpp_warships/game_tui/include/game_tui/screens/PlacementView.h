#pragma once

#include <optional>

#include <ftxui/dom/elements.hpp>

#include <game_core/Coordinate.h>
#include <game_flow/Match.h>
#include <game_tui/Theme.h>
#include <game_tui/screens/PlacementState.h>
#include <game_tui/views/BoardView.h>

namespace cpp_warships::game_tui {
    /** @brief Draws the player's board beside the fleet still waiting to be laid out.
     *  Reads the match and returns elements; it changes nothing. */
    class PlacementView {
    public:
        [[nodiscard]] ftxui::Element render(
                const Theme& theme,
                const game_flow::Match& match,
                const PlacementState& state
        );

        /** @brief The board cell drawn at a screen position, or nullopt when that is off it. */
        [[nodiscard]] std::optional<game_core::Coordinate> cellAt(int screenX, int screenY) const;

    private:
        BoardView boardView_;
    };
} // namespace cpp_warships::game_tui
