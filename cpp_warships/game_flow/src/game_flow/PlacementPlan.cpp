#include <game_flow/PlacementPlan.h>

#include <algorithm>
#include <numeric>
#include <utility>
#include <vector>

namespace cpp_warships::game_flow {
    namespace {
        /** @brief How many random attempts a single ship gets before giving up. */
        constexpr int MAXIMUM_PLACEMENT_ATTEMPTS = 200;
    } // namespace

    PlacementPlan::PlacementPlan(
            game_core::FleetComposition composition,
            const game_core::Board& board
    )
        : remaining_(composition.countsByLength()) {
        for (const game_core::Ship& ship : board.ships()) {
            const auto placed = remaining_.find(ship.length());
            if (placed != remaining_.end() && placed->second > 0) {
                --placed->second;
            }
        }
    }

    int PlacementPlan::remainingOf(int shipLength) const {
        const auto found = remaining_.find(shipLength);
        return found == remaining_.end() ? 0 : found->second;
    }

    const std::map<int, int>& PlacementPlan::remaining() const noexcept {
        return remaining_;
    }

    int PlacementPlan::remainingShipCount() const {
        const auto addRemaining = [](int total, const std::pair<const int, int>& entry) {
            return total + entry.second;
        };

        return std::accumulate(
            remaining_.begin(),
            remaining_.end(),
            0,
            addRemaining
        );
    }

    bool PlacementPlan::isComplete() const {
        return remainingShipCount() == 0;
    }

    bool placeFleetRandomly(
            game_core::Board& board,
            const game_core::FleetComposition& composition,
            RandomEngine& randomEngine
    ) {
        board.clear();

        std::vector<int> lengthsToPlace;
        for (const auto& [length, count] : composition.countsByLength()) {
            lengthsToPlace.insert(
                lengthsToPlace.end(),
                static_cast<std::size_t>(count),
                length
            );
        }
        std::sort(
            lengthsToPlace.begin(),
            lengthsToPlace.end(),
            std::greater<>()
        );

        std::uniform_int_distribution<int> columnDistribution{0, board.width() - 1};
        std::uniform_int_distribution<int> rowDistribution{0, board.height() - 1};
        std::uniform_int_distribution<int> directionDistribution{0, 1};

        for (const int length : lengthsToPlace) {
            bool isPlaced = false;
            for (int attempt = 0; attempt < MAXIMUM_PLACEMENT_ATTEMPTS && !isPlaced; ++attempt) {
                const game_core::Coordinate origin{
                        columnDistribution(randomEngine),
                        rowDistribution(randomEngine)
                };
                const auto direction = directionDistribution(randomEngine) == 0
                                               ? game_core::Direction::Horizontal
                                               : game_core::Direction::Vertical;

                const game_core::PlacementError placeError = board.place(origin, direction, length);
                isPlaced = placeError == game_core::PlacementError::None;
            }

            if (!isPlaced) {
                board.clear();
                return false;
            }
        }

        return true;
    }
} // namespace cpp_warships::game_flow
