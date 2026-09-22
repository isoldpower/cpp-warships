#include <game_tui/views/BoardView.h>

#include <algorithm>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include <game_tui/CellAppearance.h>

namespace cpp_warships::game_tui {
    namespace {
        constexpr int CELL_WIDTH = 2;
        constexpr int ROW_LABEL_WIDTH = 3;
        const std::string COLUMN_LETTERS = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

        std::string columnLabel(int column) {
            if (column >= static_cast<int>(COLUMN_LETTERS.size())) {
                return "?";
            }

            return std::string(1, COLUMN_LETTERS[static_cast<std::size_t>(column)]);
        }

        std::string rowLabel(int row) {
            const std::string number = std::to_string(row + 1);
            const int padding = ROW_LABEL_WIDTH - 1 - static_cast<int>(number.size());
            return std::string(static_cast<std::size_t>(std::max(padding, 0)), ' ') + number + " ";
        }

        ftxui::Element cellElement(
                const CellAppearance& appearance,
                const Theme& theme,
                bool isUnderCursor,
                bool isMarked,
                ftxui::Color markColor
        ) {
            const ftxui::Color foreground =
                    isUnderCursor ? theme.background : appearance.foreground;
            const ftxui::Element cell =
                    ftxui::text(appearance.glyph + " ") | ftxui::color(foreground);

            if (isUnderCursor) {
                return cell | ftxui::bgcolor(theme.cursor);
            }

            if (isMarked) {
                return cell | ftxui::bgcolor(markColor);
            }

            return cell;
        }
    } // namespace

    ftxui::Element BoardView::render(
            const game_core::Board& board,
            game_core::Visibility visibility,
            const Theme& theme,
            const BoardOverlay& overlay
    ) {
        boardWidth_ = board.width();
        boardHeight_ = board.height();

        std::vector<ftxui::Element> columnHeaders;
        std::vector<ftxui::Element> rowHeaders;
        std::vector<ftxui::Element> gridRows;

        for (int column = 0; column < boardWidth_; ++column) {
            columnHeaders.push_back(
                    ftxui::text(columnLabel(column) + " ") | ftxui::color(theme.textMuted)
            );
        }

        for (int row = 0; row < boardHeight_; ++row) {
            std::vector<ftxui::Element> cells;

            for (int column = 0; column < boardWidth_; ++column) {
                const game_core::Coordinate coordinate{column, row};
                const CellAppearance appearance =
                        appearanceOf(board.stateAt(coordinate, visibility), theme);

                cells.push_back(cellElement(
                        appearance,
                        theme,
                        overlay.cursor == coordinate,
                        overlay.marked.contains(coordinate),
                        overlay.markColor
                ));
            }

            rowHeaders.push_back(ftxui::text(rowLabel(row)) | ftxui::color(theme.textMuted));
            gridRows.push_back(ftxui::hbox(std::move(cells)));
        }

        return ftxui::vbox(
                {ftxui::hbox(
                         {ftxui::text(std::string(ROW_LABEL_WIDTH, ' ')),
                          ftxui::hbox(std::move(columnHeaders))}
                 ),
                 ftxui::hbox(
                         {ftxui::vbox(std::move(rowHeaders)),
                          ftxui::vbox(std::move(gridRows)) | ftxui::reflect(gridBox_)}
                 )}
        );
    }

    std::optional<game_core::Coordinate> BoardView::cellAt(int screenX, int screenY) const {
        if (!gridBox_.Contain(screenX, screenY)) {
            return std::nullopt;
        }

        const game_core::Coordinate coordinate{
                (screenX - gridBox_.x_min) / CELL_WIDTH,
                screenY - gridBox_.y_min
        };

        if (coordinate.x >= boardWidth_ || coordinate.y >= boardHeight_) {
            return std::nullopt;
        }

        return coordinate;
    }
} // namespace cpp_warships::game_tui
