#include <application/core/Ship.h>
#include <application/head/common/PresentationContext.h>
#include <application/head/common/render/CoordinateLabel.h>
#include <application/head/common/render/Notices.h>
#include <application/head/plain/PlainBoardGrid.h>
#include <application/head/plain/PlainFrame.h>
#include <application/head/plain/PlainPlacementView.h>

#include <string>
#include <utility>
#include <vector>

namespace cpp_warships::head::plain {
    namespace {
        /** @brief The cells the ship in hand would take up, so they can be
         * marked on the grid. */
        [[nodiscard]] std::unordered_set<core::Coordinate> shipInHandCells(
            const common::state::PlacementState& state,
            const int lengthInHand
        ) {
            if (lengthInHand <= 0) {
                return {};
            }

            const core::Ship shipInHand{state.cursor, state.direction, lengthInHand};
            const std::vector<core::Coordinate> covered = shipInHand.coordinates();
            return {covered.begin(), covered.end()};
        }

        [[nodiscard]] std::vector<std::string> rosterLines(
            const flow::PlacementPlan& plan,
            const int lengthInHand
        ) {
            std::vector<std::string> lines{"FLEET WAITING"};

            for (const auto& [length, remaining] : plan.remaining()) {
                const std::string marker = length == lengthInHand ? " <- in hand" : "";
                lines.push_back(
                    "  length " + std::to_string(length) + " : " + std::to_string(remaining) +
                    " left" + marker
                );
            }

            return lines;
        }

        /** @brief Whether a ship of @p lengthInHand may lie where @p state aims it. */
        [[nodiscard]] bool fitsWhereAimed(
            const core::Board& board,
            const common::state::PlacementState& state,
            const int lengthInHand
        ) {
            return lengthInHand > 0 &&
                   board.canPlace(state.cursor, state.direction, lengthInHand) ==
                       core::PlacementError::None;
        }

        [[nodiscard]] std::string aimingLine(
            const common::state::PlacementState& state,
            const bool isLegal
        ) {
            const std::string lie =
                state.direction == core::Direction::Horizontal ? "across" : "down";
            return "  aiming at " + common::render::coordinateLabel(state.cursor) + ", lying " +
                   lie + (isLegal ? "" : "   (will not fit here)");
        }

        [[nodiscard]] std::vector<PlainKey> placementKeys(const bool isFleetComplete) {
            std::vector<PlainKey> keys{
                {"arrows", "aim"},
                {"enter", "lay the ship"},
                {"back", "take it back"},
                {"r", "turn it"},
                {"tab", "another ship"},
                {"f", "shuffle the fleet"},
            };
            if (isFleetComplete) {
                keys.emplace_back("b", "begin the battle");
            }

            keys.emplace_back("esc", "back to the menu");
            return keys;
        }
    }  // namespace

    /** @brief The theme is offered and not taken: printed text is not dressed
     * in colour. */
    PlainPlacementView::PlainPlacementView(const common::PresentationContext& context) noexcept
        : context_(context) {}

    common::render::Frame PlainPlacementView::render(int, int) {
        const flow::Match& match = context_.game().match();
        const common::state::PlacementState& state = context_.state().placement;
        const flow::PlacementPlan plan = match.playerPlacementPlan();
        const int lengthInHand = common::state::shipLengthInHand(plan, state);
        const bool isLegal = fitsWhereAimed(match.playerBoard(), state, lengthInHand);

        const PlainBoardOverlay overlay{
            .cursor = state.cursor,
            .marked = shipInHandCells(state, lengthInHand),
            .markGlyph = isLegal ? '+' : '!'
        };

        return PlainPage{}
            .line("PLACE YOUR FLEET")
            .blank()
            .lines(plainBoardLines(match.playerBoard(), core::Visibility::Owner, overlay))
            .blank()
            .line(plainBoardLegend())
            .blank()
            .lines(rosterLines(plan, lengthInHand))
            .blank()
            .line(aimingLine(state, isLegal))
            .blank()
            .keys(placementKeys(plan.isComplete()))
            .blank()
            .notices(context_.application())
            .frame();
    }
}  // namespace cpp_warships::head::plain
