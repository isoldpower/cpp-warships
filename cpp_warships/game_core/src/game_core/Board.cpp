#include <game_core/Board.h>

#include <algorithm>
#include <optional>

namespace cpp_warships::game_core {

    Board::Board(int width, int height)
        : width_(std::max(0, width))
        , height_(std::max(0, height)) {}

    int Board::width() const noexcept {
        return width_;
    }

    int Board::height() const noexcept {
        return height_;
    }

    bool Board::contains(Coordinate coordinate) const noexcept {
        return coordinate.x >= 0 && coordinate.x < width_ && coordinate.y >= 0 &&
               coordinate.y < height_;
    }

    const Ship* Board::shipAt(Coordinate coordinate) const noexcept {
        const auto foundShip = std::find_if(
            ships_.begin(),
            ships_.end(),
            [coordinate](const Ship& ship) {
                return ship.occupies(coordinate);
            });

        return foundShip == ships_.end() ? nullptr : &*foundShip;
    }

    bool Board::touchesExistingShip(const Coordinate coordinate) const {
        for (int offsetY = -1; offsetY <= 1; ++offsetY) {
            for (int offsetX = -1; offsetX <= 1; ++offsetX) {
                if (shipAt({coordinate.x + offsetX, coordinate.y + offsetY}) != nullptr) {
                    return true;
                }
            }
        }

        return false;
    }

    PlacementError Board::canPlace(
        const Coordinate origin,
        const Direction direction,
        const int length) const {
        if (length <= 0) {
            return PlacementError::InvalidLength;
        }

        const Ship candidateShip(origin, direction, length);
        for (const Coordinate coordinate : candidateShip.coordinates()) {
            if (!contains(coordinate)) {
                return PlacementError::OutOfBounds;
            }
            if (shipAt(coordinate) != nullptr) {
                return PlacementError::Overlaps;
            }
            if (touchesExistingShip(coordinate)) {
                return PlacementError::TouchesAnotherShip;
            }
        }

        return PlacementError::None;
    }

    PlacementError Board::place(
            Coordinate origin,
            Direction direction,
            int length,
            int segmentHealth) {
        const PlacementError error = canPlace(origin, direction, length);
        if (error != PlacementError::None) {
            return error;
        }

        ships_.emplace_back(origin, direction, length, segmentHealth);
        return PlacementError::None;
    }

    bool Board::removeShipAt(Coordinate coordinate) {
        const auto foundShip = std::find_if(
            ships_.begin(),
            ships_.end(),
            [coordinate](const Ship& ship) {
                return ship.occupies(coordinate);
            });

        if (foundShip == ships_.end()) {
            return false;
        } else {
            ships_.erase(foundShip);
            return true;
        }
    }

    AttackOutcome Board::attack(Coordinate coordinate, int damage) {
        if (!contains(coordinate)) {
            return AttackOutcome::OutOfBounds;
        } else if (attackedCells_.contains(coordinate)) {
            return AttackOutcome::AlreadyAttacked;
        }

        attackedCells_.insert(coordinate);
        const auto targetShip = std::find_if(
            ships_.begin(),
            ships_.end(),
            [coordinate](const Ship& ship) {
                return ship.occupies(coordinate);
            });

        if (targetShip == ships_.end()) {
            return AttackOutcome::Miss;
        } else {
            const std::optional<int> index = targetShip->segmentIndexAt(coordinate);
            targetShip->damageSegment(*index, damage);

            return targetShip->isSunk() ? AttackOutcome::Sunk: AttackOutcome::Hit;
        }
    }

    CellState Board::stateAt(Coordinate coordinate, Visibility visibility) const {
        const bool isAttacked = attackedCells_.contains(coordinate);
        const Ship* ship = shipAt(coordinate);

        if (ship != nullptr && isAttacked) {
            return ship->isSunk() ? CellState::Sunk : CellState::Hit;
        } else if (isAttacked) {
            return CellState::Miss;
        } else if (ship != nullptr && visibility == Visibility::Owner) {
            return CellState::Ship;
        }

        return CellState::Water;
    }

    bool Board::hasShipWithin(Coordinate center, int radius) const {
        for (int offsetY = -radius; offsetY <= radius; ++offsetY) {
            for (int offsetX = -radius; offsetX <= radius; ++offsetX) {
                const Coordinate probeCoordinate{center.x + offsetX, center.y + offsetY};
                if (contains(probeCoordinate) && shipAt(probeCoordinate) != nullptr) {
                    return true;
                }
            }
        }

        return false;
    }

    bool Board::allShipsSunk() const {
        return !ships_.empty() && std::all_of(
            ships_.begin(),
            ships_.end(),
            [](const Ship& ship) {
                return ship.isSunk();
            });
    }

    bool Board::hasShips() const noexcept {
        return !ships_.empty();
    }

    const std::vector<Ship>& Board::ships() const noexcept {
        return ships_;
    }

    const std::unordered_set<Coordinate>& Board::attackedCells() const noexcept {
        return attackedCells_;
    }

    void Board::clear() noexcept {
        ships_.clear();
        attackedCells_.clear();
    }
} // namespace cpp_warships::game_core
