#include <game_tui/screens/PlacementState.h>

namespace cpp_warships::game_tui {
    namespace {
        int longestLengthLeft(const game_flow::PlacementPlan& plan) {
            int longest = 0;
            for (const auto& [length, remaining] : plan.remaining()) {
                if (remaining > 0) {
                    longest = length;
                }
            }

            return longest;
        }

        int shortestLengthLeft(const game_flow::PlacementPlan& plan) {
            for (const auto& [length, remaining] : plan.remaining()) {
                if (remaining > 0) {
                    return length;
                }
            }

            return 0;
        }
    } // namespace

    int shipLengthInHand(const game_flow::PlacementPlan& plan, const PlacementState& state) {
        if (plan.remainingOf(state.preferredShipLength) > 0) {
            return state.preferredShipLength;
        }

        return longestLengthLeft(plan);
    }

    int nextShipLength(const game_flow::PlacementPlan& plan, int currentLength) {
        for (const auto& [length, remaining] : plan.remaining()) {
            if (remaining > 0 && length > currentLength) {
                return length;
            }
        }

        return shortestLengthLeft(plan);
    }
} // namespace cpp_warships::game_tui
