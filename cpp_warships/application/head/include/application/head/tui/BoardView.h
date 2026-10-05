#pragma once

#include <application/core/Coordinate.h>
#include <application/core/Outcomes.h>
#include <application/head/common/render/CoordinateLabel.h>

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/box.hpp>
#include <ftxui/screen/color.hpp>
#include <optional>
#include <string>
#include <unordered_set>

namespace cpp_warships::core {
    class Board;
}

namespace cpp_warships::head::common {
    struct Theme;
}

namespace cpp_warships::head::common::input {
    class GridGeometry;
}

namespace cpp_warships::head::tui {
    /** @brief What is drawn on top of a board: where the cursor rests and which cells are
     * marked. */
    struct BoardOverlay {
        std::optional<core::Coordinate> cursor{};
        std::unordered_set<core::Coordinate> marked{};
        common::CellColors markColors{};
    };

    /** @brief Draws a board as a labelled grid and remembers where that grid landed. */
    class BoardView {
    public:
        /** @brief Draws into @p geometry's record of @p region, which must
         * outlive it. */
        BoardView(
            common::input::GridGeometry& geometry,
            common::input::ScreenRegion region
        ) noexcept;

        [[nodiscard]] ftxui::Element render(
            const core::Board& board,
            core::Visibility visibility,
            const common::Theme& theme,
            const BoardOverlay& overlay
        );

        /** @brief Writes down where the grid landed when it was last laid out. */
        void publishGeometry() const;

    private:
        common::input::GridGeometry& geometry_;
        common::input::ScreenRegion region_;
        ftxui::Box gridBox_;
        int boardWidth_ = 0;
        int boardHeight_ = 0;
    };
}  // namespace cpp_warships::head::tui
