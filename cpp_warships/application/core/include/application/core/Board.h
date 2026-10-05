#pragma once

#include <application/core/Coordinate.h>
#include <application/core/Direction.h>
#include <application/core/Outcomes.h>
#include <application/core/Ship.h>

#include <optional>
#include <unordered_set>
#include <vector>

namespace cpp_warships::core {
    /** @brief A player's grid: the ships on it and the cells attacked so far.
     * Owns its ships by value and performs no input or output. */
    class Board {
    public:
        Board(int width, int height);

        /** @brief Rebuilds a board with ships and shots already on it, as when loading a save. */
        Board(
            int width,
            int height,
            std::vector<Ship> ships,
            std::unordered_set<Coordinate> attackedCells
        );

        [[nodiscard]] int width() const noexcept;
        [[nodiscard]] int height() const noexcept;
        [[nodiscard]] bool contains(Coordinate coordinate) const noexcept;

        /** @brief Whether a ship of @p length fits at @p origin, and why not when it does not. */
        [[nodiscard]] PlacementError canPlace(
            Coordinate origin,
            Direction direction,
            int length
        ) const;

        /** @brief Places a ship when the placement is legal.
         * @return PlacementError::None on success, leaving the board untouched otherwise. */
        PlacementError place(
            Coordinate origin,
            Direction direction,
            int length,
            int segmentHealth = DEFAULT_SEGMENT_HEALTH
        );

        /** @brief Removes the ship covering @p coordinate; false when no ship is there. */
        bool removeShipAt(Coordinate coordinate);

        /** @brief Attacks a cell, which may be struck again while a segment there still lives. */
        AttackOutcome attack(Coordinate coordinate, int damage);

        /** @brief What @p visibility knows about @p coordinate. */
        [[nodiscard]] CellState stateAt(Coordinate coordinate, Visibility visibility) const;

        /** @brief How many hit points the ship segment at @p coordinate has left, when
         * @p visibility may know it: always for the owner, only once it was hit otherwise. */
        [[nodiscard]] std::optional<int> healthAt(
            Coordinate coordinate,
            Visibility visibility
        ) const;

        /** @brief Whether any ship stands within @p radius of @p center, ignoring fog. */
        [[nodiscard]] bool hasShipWithin(Coordinate center, int radius) const;

        /** @brief Whether the board holds ships and every one of them is sunk.
         * An empty board reports false: it is not set up yet, rather than lost. */
        [[nodiscard]] bool allShipsSunk() const;
        [[nodiscard]] bool hasShips() const noexcept;
        [[nodiscard]] const std::vector<Ship>& ships() const noexcept;
        [[nodiscard]] const std::unordered_set<Coordinate>& attackedCells() const noexcept;

        void clear() noexcept;

    private:
        [[nodiscard]] const Ship* shipAt(Coordinate coordinate) const noexcept;
        [[nodiscard]] bool isOutside(Coordinate coordinate) const noexcept;
        [[nodiscard]] bool isTaken(Coordinate coordinate) const noexcept;
        [[nodiscard]] bool touchesExistingShip(Coordinate coordinate) const;

        /** @brief What a struck cell of @p ship shows: sunk with its ship, or its segment's damage.
         */
        [[nodiscard]] static CellState stateOfStruck(const Ship& ship, Coordinate coordinate);

        /** @brief Marks the water hugging @p ship as attacked since nothing can be hiding there. */
        void revealWaterAround(const Ship& ship);
        /** @brief Processes the attack when it is known that the attacked cell is a ship. */
        AttackOutcome attackShipCell(Ship* targetShip, Coordinate coordinate, int damage);

        int width_;
        int height_;
        std::vector<Ship> ships_;
        std::unordered_set<Coordinate> attackedCells_;
    };
}  // namespace cpp_warships::core
