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
#include <optional>
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

        /** @brief Everything a board is drawn from: the board, how much of it may be seen, the
         * theme it is painted in and what lies on top of it. */
        struct BoardDrawing {
            const core::Board& board;
            core::Visibility visibility;
            const common::Theme& theme;
            const BoardOverlay& overlay;
        };

        /** @brief The colours a cell is painted in, with the overlay having its say first. */
        common::CellColors colorsOf(
            const BoardDrawing& drawing,
            const core::Coordinate coordinate
        ) {
            if (drawing.overlay.marked.contains(coordinate)) {
                return drawing.overlay.markColors;
            }
            if (drawing.overlay.cursor == coordinate) {
                return drawing.theme.cursor;
            }

            const core::CellState state = drawing.board.stateAt(coordinate, drawing.visibility);
            return appearanceOf(state, drawing.theme).colors;
        }

        /** @brief What a cell shows: the hit points left on a struck ship still afloat, and
         * otherwise the glyph of what is known to be there. */
        std::string glyphOf(const BoardDrawing& drawing, const core::Coordinate coordinate) {
            if (drawing.overlay.marked.contains(coordinate)) {
                return " ";
            }

            const core::CellState state = drawing.board.stateAt(coordinate, drawing.visibility);
            const std::optional<int> health =
                drawing.board.healthAt(coordinate, drawing.visibility);
            const bool isHitAfloat =
                state == core::CellState::Damaged || state == core::CellState::Destroyed;
            if (isHitAfloat && health.has_value()) {
                return std::to_string(*health);
            }

            return appearanceOf(state, drawing.theme).glyph;
        }

        ftxui::Element tileElement(const std::string& glyph, const common::CellColors& colors) {
            return ftxui::text(centredInTile(glyph)) | color(colors.ink) | bgcolor(colors.fill);
        }

        ftxui::Element spacer(int width, const common::Theme& theme) {
            return ftxui::text(std::string(static_cast<std::size_t>(width), ' ')) |
                   bgcolor(theme.background);
        }

        /** @brief The letters over the columns, each centred over its tile. */
        ftxui::Element columnHeaders(const int boardWidth, const common::Theme& theme) {
            std::vector<ftxui::Element> headers{ftxui::text(std::string(ROW_LABEL_WIDTH, ' '))};
            for (int column = 0; column < boardWidth; ++column) {
                const bool isLast = column + 1 == boardWidth;
                const auto trailing =
                    static_cast<std::size_t>(isLast ? 0 : BOARD_COLUMN_PITCH - BOARD_TILE_WIDTH);
                const std::string label =
                    centredInTile(common::render::columnLabel(column)) + std::string(trailing, ' ');
                headers.push_back(ftxui::text(label) | color(theme.textMuted));
            }

            return ftxui::hbox(std::move(headers));
        }

        /** @brief The numbers down the side, with a blank line beside each gap between rows. */
        ftxui::Element rowHeaders(const int boardHeight, const common::Theme& theme) {
            std::vector<ftxui::Element> headers;
            for (int row = 0; row < boardHeight; ++row) {
                headers.push_back(ftxui::text(rowLabel(row)) | color(theme.textMuted));
                if (row + 1 < boardHeight) {
                    headers.push_back(ftxui::text(std::string(ROW_LABEL_WIDTH, ' ')));
                }
            }

            return ftxui::vbox(std::move(headers));
        }

        /** @brief One row of tiles, with a gap after every tile but the last. */
        ftxui::Element tileRow(const BoardDrawing& drawing, const int row) {
            const int boardWidth = drawing.board.width();
            std::vector<ftxui::Element> cells;
            for (int column = 0; column < boardWidth; ++column) {
                const core::Coordinate coordinate{column, row};
                cells.push_back(
                    tileElement(glyphOf(drawing, coordinate), colorsOf(drawing, coordinate))
                );
                if (column + 1 < boardWidth) {
                    cells.push_back(spacer(BOARD_TILE_GAP, drawing.theme));
                }
            }

            return ftxui::hbox(std::move(cells));
        }

        /** @brief Every row of tiles, with a blank row between each two. */
        ftxui::Element tileRows(const BoardDrawing& drawing) {
            const int boardHeight = drawing.board.height();
            std::vector<ftxui::Element> rows;
            for (int row = 0; row < boardHeight; ++row) {
                rows.push_back(tileRow(drawing, row));
                if (row + 1 < boardHeight) {
                    rows.push_back(spacer(gridWidthOf(drawing.board.width()), drawing.theme));
                }
            }

            return ftxui::vbox(std::move(rows));
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
        const BoardDrawing drawing{
            .board = board,
            .visibility = visibility,
            .theme = theme,
            .overlay = overlay
        };

        return ftxui::vbox(
            {columnHeaders(boardWidth_, theme),
             ftxui::hbox(
                 {rowHeaders(boardHeight_, theme), tileRows(drawing) | reflectWholeBox(gridBox_)}
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
