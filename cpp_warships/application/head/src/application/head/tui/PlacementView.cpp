#include <application/core/Ship.h>
#include <application/head/common/PresentationContext.h>
#include <application/head/tui/CellAppearance.h>
#include <application/head/tui/FtxuiNotices.h>
#include <application/head/tui/FtxuiPalette.h>
#include <application/head/tui/KeyHint.h>
#include <application/head/tui/PlacementView.h>
#include <application/head/tui/ScrollPanel.h>

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace cpp_warships::head::tui {
    namespace {
        /** @brief How wide the fleet panel stands, whatever the terminal does. */
        constexpr int PLACEMENT_PANEL_WIDTH = 28;

        /** @brief How a ship is drawn on the roster: smaller than a board cell,
         * with a gap across and a clear line between one ship and the next. */
        constexpr int ROSTER_TILE_WIDTH = 2;
        constexpr int ROSTER_TILE_GAP = 1;
        constexpr int ROSTER_ROW_GAP = 1;

        std::unordered_set<core::Coordinate> shipInHandCells(
            const common::state::PlacementState& state,
            int lengthInHand
        ) {
            if (lengthInHand <= 0) {
                return {};
            }

            const core::Ship shipInHand{state.cursor, state.direction, lengthInHand};
            const std::vector<core::Coordinate> covered = shipInHand.coordinates();
            return {covered.begin(), covered.end()};
        }

        common::CellColors shipInHandColors(
            const core::Board& board,
            const common::state::PlacementState& state,
            int lengthInHand,
            const common::Theme& theme
        ) {
            const core::PlacementError error =
                board.canPlace(state.cursor, state.direction, lengthInHand);
            const bool isLegal = error == core::PlacementError::None;
            return {isLegal ? theme.success : theme.danger, theme.background};
        }

        /** @brief A ship of @p length as a row of separate cells, in miniature. */
        ftxui::Element hullOfLength(int length, common::Color fill) {
            const auto tileWidth = static_cast<std::size_t>(ROSTER_TILE_WIDTH);
            const auto gapWidth = static_cast<std::size_t>(ROSTER_TILE_GAP);

            std::vector<ftxui::Element> tiles;
            for (int segment = 0; segment < length; ++segment) {
                if (segment > 0) {
                    tiles.push_back(ftxui::text(std::string(gapWidth, ' ')));
                }

                tiles.push_back(ftxui::text(std::string(tileWidth, ' ')) | bgcolor(fill));
            }

            return ftxui::hbox(std::move(tiles));
        }

        ftxui::Element header(const common::Theme& theme, const flow::Match& match) {
            const std::string boardSize = std::to_string(match.settings().boardSize());

            return ftxui::hbox(
                {ftxui::text("PLACE YOUR FLEET") | ftxui::bold | color(theme.accent),
                 ftxui::filler(),
                 ftxui::text(boardSize + " x " + boardSize) | color(theme.textMuted)}
            );
        }

        ftxui::Element fleetRoster(
            const common::Theme& theme,
            const flow::PlacementPlan& plan,
            int lengthInHand
        ) {
            std::vector<ftxui::Element> rows;

            bool isFirstEntry = true;
            for (const auto& [length, remaining] : plan.remaining()) {
                const bool isInHand = length == lengthInHand;
                const common::Color hullFill = remaining > 0 ? theme.ship.fill : theme.surface;

                for (int gap = 0; gap < ROSTER_ROW_GAP && !isFirstEntry; ++gap) {
                    rows.push_back(ftxui::text(""));
                }
                isFirstEntry = false;

                rows.push_back(
                    ftxui::hbox(
                        {ftxui::text(isInHand ? "> " : "  ") | color(theme.accent),
                         hullOfLength(length, hullFill),
                         ftxui::filler(),
                         ftxui::text("  x" + std::to_string(remaining)) | color(theme.textMuted)}
                    )
                );
            }

            return ftxui::vbox(std::move(rows));
        }

        ftxui::Element standing(
            const common::Theme& theme,
            const flow::PlacementPlan& plan,
            const common::state::PlacementState& state
        ) {
            if (plan.isComplete()) {
                return ftxui::text("fleet ready") | ftxui::bold | color(theme.success);
            }

            const bool isAcross = state.direction == core::Direction::Horizontal;
            return ftxui::text(isAcross ? "lying across" : "lying down") | color(theme.textMuted);
        }

        ftxui::Element divider(const common::Theme& theme) {
            return ftxui::separator() | color(theme.border);
        }

        ftxui::Element sectionHeading(const common::Theme& theme, const std::string& title) {
            return ftxui::text(title) | ftxui::bold | color(theme.accent);
        }

        std::vector<KeyHint> legend(const flow::PlacementPlan& plan) {
            std::vector<KeyHint> hints{
                KeyHint{.key = "arrows", .description = "aim"},
                KeyHint{.key = "enter", .description = "lay the ship"},
                KeyHint{.key = "bksp", .description = "take it back"},
                KeyHint{.key = "r", .description = "turn it"},
                KeyHint{.key = "tab", .description = "another ship"},
                KeyHint{.key = "f", .description = "shuffle the fleet"}
            };

            if (plan.isComplete()) {
                hints.push_back(KeyHint{.key = "b", .description = "begin the battle"});
            }

            hints.push_back(KeyHint{.key = "esc", .description = "back to the menu"});
            return hints;
        }
    }  // namespace

    PlacementView::PlacementView(
        const common::PresentationContext& context,
        common::input::GridGeometry& geometry
    ) noexcept
        : context_(context)
        , geometry_(geometry)
        , boardView_(geometry, common::input::ScreenRegion::OwnWaters)
        , boardPanel_(common::input::ScreenRegion::OwnWaters)
        , fleetPanel_(common::input::ScreenRegion::Fleet)
        , shortcutsPanel_(common::input::ScreenRegion::Shortcuts) {}

    void PlacementView::publishLayout() {
        boardView_.publishGeometry();
        for (const ScrollPanel* panel : {&boardPanel_, &fleetPanel_, &shortcutsPanel_}) {
            panel->publish(geometry_);
        }
        hotspots_.publish(geometry_);
    }

    ftxui::Element PlacementView::renderElement() {
        const common::Theme& theme = context_.theme();
        const flow::Match& match = context_.game().match();
        const common::state::PlacementState& state = context_.state().placement;
        constexpr common::ScreenKind SCREEN = common::ScreenKind::Placement;

        const flow::PlacementPlan plan = match.playerPlacementPlan();
        const core::Board& board = match.playerBoard();
        const int lengthInHand = common::state::shipLengthInHand(plan, state);

        const BoardOverlay overlay{
            .cursor = state.cursor,
            .marked = shipInHandCells(state, lengthInHand),
            .markColors = shipInHandColors(board, state, lengthInHand, theme)
        };

        ftxui::Element waters = boardPanel_.render(
            context_,
            SCREEN,
            ftxui::text("YOUR WATERS") | ftxui::bold | color(theme.textMuted),
            boardView_.render(board, core::Visibility::Owner, theme, overlay) | ftxui::center
        );
        ftxui::Element fleet = fleetPanel_.render(
            context_,
            SCREEN,
            sectionHeading(theme, "FLEET"),
            ftxui::vbox(
                {fleetRoster(theme, plan, lengthInHand),
                 divider(theme),
                 standing(theme, plan, state)}
            )
        );
        ftxui::Element shortcuts = shortcutsPanel_.render(
            context_,
            SCREEN,
            sectionHeading(theme, "KEYS"),
            keyLegend(theme, legend(plan), hotspots_, !context_.state().isKeyboardLayoutFree)
        );

        ftxui::Element body =
            isNarrow()
                ? ftxui::vbox(
                      {std::move(waters) | ftxui::yflex_grow |
                           ftxui::yflex_shrink_factor(BOARD_PANEL_SHRINK_WEIGHT),
                       divider(theme),
                       std::move(fleet),
                       divider(theme),
                       std::move(shortcuts)}
                  )
                : ftxui::hbox(
                      {std::move(waters) | ftxui::flex,
                       divider(theme),
                       ftxui::vbox(
                           {std::move(fleet), divider(theme), std::move(shortcuts), ftxui::filler()}
                       ) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, PLACEMENT_PANEL_WIDTH)}
                  );

        return ftxui::vbox(
                   {header(theme, match),
                    divider(theme),
                    std::move(body) | ftxui::flex,
                    noticeBlock(theme, context_.application())}
               ) |
               ftxui::border | color(theme.border) | bgcolor(theme.background) | ftxui::flex;
    }
}  // namespace cpp_warships::head::tui
