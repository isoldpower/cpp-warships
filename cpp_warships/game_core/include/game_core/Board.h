#pragma once

#include <unordered_set>
#include <vector>

#include <game_core/Coordinate.h>
#include <game_core/Direction.h>
#include <game_core/Outcomes.h>
#include <game_core/Ship.h>

namespace cpp_warships::game_core {
    /** @brief A player's grid: the ships on it and the cells attacked so far.
     *  Owns its ships by value and performs no input or output. */
    class Board {
    public:
        Board(int width, int height);

        /** @brief Rebuilds a board with ships and shots already on it, as when loading a save. */
        Board(int width,
              int height,
              std::vector<Ship> ships,
              std::unordered_set<Coordinate> attackedCells);

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
         *  @return PlacementError::None on success, leaving the board untouched otherwise. */
        PlacementError place(
                Coordinate origin,
                Direction direction,
                int length,
                int segmentHealth = DEFAULT_SEGMENT_HEALTH
        );

        /** @brief Removes the ship covering @p coordinate; false when no ship is there. */
        bool removeShipAt(Coordinate coordinate);

        /** @brief Attacks a cell, which may be struck again while a segment there still lives.
         *  Resolved cells -- water already shot, or a destroyed segment -- reject further shots. */
        AttackOutcome attack(Coordinate coordinate, int damage);

        /** @brief What @p visibility knows about @p coordinate. */
        [[nodiscard]] CellState stateAt(Coordinate coordinate, Visibility visibility) const;

        /** @brief Whether any ship stands within @p radius of @p center, ignoring fog. */
        [[nodiscard]] bool hasShipWithin(Coordinate center, int radius) const;

        /** @brief Whether the board holds ships and every one of them is sunk.
         *  An empty board reports false: it is not set up yet, rather than lost. */
        [[nodiscard]] bool allShipsSunk() const;
        [[nodiscard]] bool hasShips() const noexcept;
        [[nodiscard]] const std::vector<Ship>& ships() const noexcept;
        [[nodiscard]] const std::unordered_set<Coordinate>& attackedCells() const noexcept;

        void clear() noexcept;

    private:
        [[nodiscard]] const Ship* shipAt(Coordinate coordinate) const noexcept;
        [[nodiscard]] bool touchesExistingShip(Coordinate coordinate) const;

        int width_;
        int height_;
        std::vector<Ship> ships_;
        std::unordered_set<Coordinate> attackedCells_;
    };
} // namespace cpp_warships::game_core
