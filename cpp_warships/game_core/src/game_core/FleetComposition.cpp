#include <game_core/FleetComposition.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <numeric>
#include <utility>

namespace cpp_warships::game_core {
    namespace {
        /** @brief Share of the board the fleet is allowed to cover. */
        constexpr float SHIP_COVERED_AREA_RATIO = 0.2F;
        /** @brief Board size the base counts below are calibrated for. */
        constexpr float REFERENCE_BOARD_SIZE = 10.0F;
        constexpr std::array<int, 4> SHIP_LENGTHS{4, 3, 2, 1};
        constexpr std::array<int, 4> BASE_SHIP_COUNTS{1, 2, 3, 4};
    } // namespace

    FleetComposition::FleetComposition(std::map<int, int> countsByLength)
        : countsByLength_(std::move(countsByLength)) {}

    FleetComposition FleetComposition::forBoardSize(int boardSize) {
        if (boardSize <= 0) {
            return FleetComposition{};
        }

        const auto boardSizeAsFloat = static_cast<float>(boardSize);
        const float scaleFactor = boardSizeAsFloat / REFERENCE_BOARD_SIZE;
        int remainingCells =
                static_cast<int>(boardSizeAsFloat * boardSizeAsFloat * SHIP_COVERED_AREA_RATIO);

        std::map<int, int> counts;
        for (std::size_t index = 0; index < SHIP_LENGTHS.size(); ++index) {
            const int length = SHIP_LENGTHS[index];
            const auto shipCounts = static_cast<float>(BASE_SHIP_COUNTS[index]);
            const auto scaledCount = static_cast<int>(shipCounts * scaleFactor);
            const int affordableCount = std::max(0, remainingCells / length);
            const int count = std::max(0, std::min(scaledCount, affordableCount));

            counts[length] = count;
            remainingCells -= count * length;
        }

        return FleetComposition{std::move(counts)};
    }

    const std::map<int, int>& FleetComposition::countsByLength() const noexcept {
        return countsByLength_;
    }

    int FleetComposition::countOf(int length) const {
        const auto foundEntry = countsByLength_.find(length);
        return foundEntry == countsByLength_.end() ? 0 : foundEntry->second;
    }

    int FleetComposition::totalShips() const {
        const auto addShipCount = [](int sum, const std::pair<const int, int>& entry) {
            return sum + entry.second;
        };

        return std::accumulate(countsByLength_.begin(), countsByLength_.end(), 0, addShipCount);
    }

    int FleetComposition::totalCells() const {
        const auto addOccupiedCells = [](int sum, const std::pair<const int, int>& entry) {
            return sum + entry.first * entry.second;
        };

        return std::accumulate(countsByLength_.begin(), countsByLength_.end(), 0, addOccupiedCells);
    }

    bool FleetComposition::isEmpty() const noexcept {
        return countsByLength_.empty();
    }
} // namespace cpp_warships::game_core
