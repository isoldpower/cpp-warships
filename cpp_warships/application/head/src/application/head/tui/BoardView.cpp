#include <application/core/Board.h>
#include <application/head/common/Theme.h>
#include <application/head/common/input/GridGeometry.h>
#include <application/head/common/render/CoordinateLabel.h>
#include <application/head/tui/BoardView.h>
#include <application/head/tui/CellAppearance.h>
#include <application/head/tui/FtxuiPalette.h>
#include <application/head/tui/ScrollPanel.h>

#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace cpp_warships::head::tui {
    namespace {
        constexpr int ROW_LABEL_WIDTH = 3;
        std::string rowLabel(int row) {
            const std::string number = std::to_string(row + 1);
            const int padding = ROW_LABEL_WIDTH - 1 - static_cast<int>(number.size());
            return std::string(static_cast<std::size_t>(std::max(padding, 0)), ' ') + number + " ";
        }

        /** @brief How many columns a whole grid of @p boardWidth cells takes
         * up. */
        int gridWidthOf(int boardWidth) {
            return boardWidth * BOARD_COLUMN_PITCH - BOARD_TILE_GAP;
        }

        /** @brief The colours a cell is painted in, with the overlay having its
         * say first. */
        common::CellColors colorsOf(
            const core::Board& board,
            core::Coordinate coordinate,
            core::Visibility visibility,
            const common::Theme& theme,
            const BoardOverlay& overlay
        ) {
            if (overlay.marked.contains(coordinate)) {
                return overlay.markColors;
            }
            if (overlay.cursor == coordinate) {
                return theme.cursor;
            }

            return appearanceOf(board.stateAt(coordinate, visibility), theme).colors;
        }

        std::string glyphOf(
            const core::Board& board,
            core::Coordinate coordinate,
            core::Visibility visibility,
            const common::Theme& theme,
            const BoardOverlay& overlay
        ) {
            if (overlay.marked.contains(coordinate)) {
                return " ";
            }

            return appearanceOf(board.stateAt(coordinate, visibility), theme).glyph;
        }

        ftxui::Element tileElement(const std::string& glyph, const common::CellColors& colors) {
            return ftxui::text(centredInTile(glyph)) | color(colors.ink) | bgcolor(colors.fill);
        }

        ftxui::Element spacer(int width, const common::Theme& theme) {
            return ftxui::text(std::string(static_cast<std::size_t>(width), ' ')) |
                   bgcolor(theme.background);
        }
    }  // namespace

    BoardView::BoardView(
        common::input::GridGeometry& geometry,
        const common::input::ScreenRegion region
    ) noexcept
        : geometry_(geometry)
        , region_(region) {}

    ftxui::Element BoardView::render(
        const core::Board& board,
        core::Visibility visibility,
        const common::Theme& theme,
        const BoardOverlay& overlay
    ) {
        boardWidth_ = board.width();
        boardHeight_ = board.height();

        std::vector<ftxui::Element> columnHeaders;
        std::vector<ftxui::Element> rowHeaders;
        std::vector<ftxui::Element> gridRows;

        for (int column = 0; column < boardWidth_; ++column) {
            const bool isLast = column + 1 == boardWidth_;
            const auto trailing =
                static_cast<std::size_t>(isLast ? 0 : BOARD_COLUMN_PITCH - BOARD_TILE_WIDTH);
            columnHeaders.push_back(
                ftxui::text(
                    centredInTile(common::render::columnLabel(column)) + std::string(trailing, ' ')
                ) |
                color(theme.textMuted)
            );
        }

        for (int row = 0; row < boardHeight_; ++row) {
            std::vector<ftxui::Element> cells;

            for (int column = 0; column < boardWidth_; ++column) {
                const core::Coordinate coordinate{column, row};
                cells.push_back(tileElement(
                    glyphOf(board, coordinate, visibility, theme, overlay),
                    colorsOf(board, coordinate, visibility, theme, overlay)
                ));

                if (column + 1 < boardWidth_) {
                    cells.push_back(spacer(BOARD_TILE_GAP, theme));
                }
            }

            rowHeaders.push_back(ftxui::text(rowLabel(row)) | color(theme.textMuted));
            gridRows.push_back(ftxui::hbox(std::move(cells)));

            if (row + 1 < boardHeight_) {
                rowHeaders.push_back(ftxui::text(std::string(ROW_LABEL_WIDTH, ' ')));
                gridRows.push_back(spacer(gridWidthOf(boardWidth_), theme));
            }
        }

        return ftxui::vbox(
            {ftxui::hbox(
                 {ftxui::text(std::string(ROW_LABEL_WIDTH, ' ')),
                  ftxui::hbox(std::move(columnHeaders))}
             ),
             ftxui::hbox(
                 {ftxui::vbox(std::move(rowHeaders)),
                  ftxui::vbox(std::move(gridRows)) | reflectWholeBox(gridBox_)}
             )}
        );
    }

    void BoardView::publishGeometry() const {
        if (boardWidth_ <= 0 || boardHeight_ <= 0 || gridBox_.x_max < gridBox_.x_min) {
            return;
        }

        geometry_.rememberBoard(
            region_,
            gridBox_.x_min,
            gridBox_.y_min,
            gridBox_.x_max - gridBox_.x_min + 1,
            gridBox_.y_max - gridBox_.y_min + 1,
            boardWidth_,
            boardHeight_,
            BOARD_COLUMN_PITCH,
            BOARD_ROW_PITCH
        );
    }
}  // namespace cpp_warships::head::tui
