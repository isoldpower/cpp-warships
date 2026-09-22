#include <game_tui/screens/PlacementView.h>

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

#include <game_core/Ship.h>
#include <game_tui/views/KeyHint.h>

namespace cpp_warships::game_tui {
    namespace {
        std::unordered_set<game_core::Coordinate> shipInHandCells(
                const PlacementState& state,
                int lengthInHand
        ) {
            if (lengthInHand <= 0) {
                return {};
            }

            const game_core::Ship shipInHand{state.cursor, state.direction, lengthInHand};
            const std::vector<game_core::Coordinate> covered = shipInHand.coordinates();
            return {covered.begin(), covered.end()};
        }

        ftxui::Color shipInHandColor(
                const game_core::Board& board,
                const PlacementState& state,
                int lengthInHand,
                const Theme& theme
        ) {
            const game_core::PlacementError error =
                    board.canPlace(state.cursor, state.direction, lengthInHand);
            return error == game_core::PlacementError::None ? theme.success : theme.danger;
        }

        std::string hullOfLength(int length) {
            std::string hull;
            for (int segment = 0; segment < length; ++segment) {
                hull += "█";
            }

            return hull;
        }

        ftxui::Element header(const Theme& theme, const game_flow::Match& match) {
            const std::string boardSize = std::to_string(match.settings().boardSize());

            return ftxui::hbox(
                    {ftxui::text("PLACE YOUR FLEET") | ftxui::bold | ftxui::color(theme.accent),
                     ftxui::filler(),
                     ftxui::text(boardSize + " x " + boardSize) | ftxui::color(theme.textMuted)}
            );
        }

        ftxui::Element fleetRoster(
                const Theme& theme,
                const game_flow::PlacementPlan& plan,
                int lengthInHand
        ) {
            std::vector<ftxui::Element> rows{
                    ftxui::text("FLEET") | ftxui::bold | ftxui::color(theme.accent)
            };

            for (const auto& [length, remaining] : plan.remaining()) {
                const bool isInHand = length == lengthInHand;
                const ftxui::Color hullColor = remaining > 0 ? theme.ship : theme.textMuted;

                rows.push_back(
                        ftxui::hbox(
                                {ftxui::text(isInHand ? "> " : "  ") | ftxui::color(theme.accent),
                                 ftxui::text(hullOfLength(length)) | ftxui::color(hullColor),
                                 ftxui::filler(),
                                 ftxui::text("  x" + std::to_string(remaining)) |
                                         ftxui::color(theme.textMuted)}
                        )
                );
            }

            return ftxui::vbox(std::move(rows));
        }

        ftxui::Element standing(
                const Theme& theme,
                const game_flow::PlacementPlan& plan,
                const PlacementState& state
        ) {
            if (plan.isComplete()) {
                return ftxui::text("fleet ready") | ftxui::bold | ftxui::color(theme.success);
            }

            const bool isAcross = state.direction == game_core::Direction::Horizontal;
            return ftxui::text(isAcross ? "lying across" : "lying down") |
                   ftxui::color(theme.textMuted);
        }

        ftxui::Element legend(const Theme& theme, const game_flow::PlacementPlan& plan) {
            std::vector<ftxui::Element> hints{
                    keyHint(theme, "arrows", "aim"),
                    keyHint(theme, "enter", "lay the ship"),
                    keyHint(theme, "bksp", "take it back"),
                    keyHint(theme, "r", "turn it"),
                    keyHint(theme, "tab", "another ship"),
                    keyHint(theme, "s", "shuffle the fleet")
            };

            if (plan.isComplete()) {
                hints.push_back(keyHint(theme, "b", "begin the battle"));
            }

            hints.push_back(keyHint(theme, "esc", "back to the menu"));
            return ftxui::vbox(std::move(hints));
        }
    } // namespace

    ftxui::Element PlacementView::render(
            const Theme& theme,
            const game_flow::Match& match,
            const PlacementState& state
    ) {
        const game_flow::PlacementPlan plan = match.playerPlacementPlan();
        const game_core::Board& board = match.playerBoard();
        const int lengthInHand = shipLengthInHand(plan, state);

        const BoardOverlay overlay{
                .cursor = state.cursor,
                .marked = shipInHandCells(state, lengthInHand),
                .markColor = shipInHandColor(board, state, lengthInHand, theme)
        };

        return ftxui::vbox(
                       {header(theme, match),
                        ftxui::separator() | ftxui::color(theme.border),
                        ftxui::hbox(
                                {boardView_.render(
                                         board,
                                         game_core::Visibility::Owner,
                                         theme,
                                         overlay
                                 ),
                                 ftxui::separator() | ftxui::color(theme.border),
                                 ftxui::vbox(
                                         {fleetRoster(theme, plan, lengthInHand),
                                          ftxui::separator() | ftxui::color(theme.border),
                                          standing(theme, plan, state),
                                          ftxui::separator() | ftxui::color(theme.border),
                                          legend(theme, plan)}
                                 )}
                        )}
               ) |
               ftxui::border | ftxui::color(theme.border) | ftxui::bgcolor(theme.background) |
               ftxui::center;
    }

    std::optional<game_core::Coordinate> PlacementView::cellAt(int screenX, int screenY) const {
        return boardView_.cellAt(screenX, screenY);
    }
} // namespace cpp_warships::game_tui
