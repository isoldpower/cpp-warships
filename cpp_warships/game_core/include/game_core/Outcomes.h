#pragma once

namespace cpp_warships::game_core {
    /**
     * @brief Result of attacking a single cell.
     */
    enum class AttackOutcome {
        Miss,
        Hit,
        Sunk,
        AlreadyAttacked,
        OutOfBounds,
    };

    /**
     * @brief Why a ship cannot be placed, or None when it can.
     */
    enum class PlacementError {
        None,
        OutOfBounds,
        Overlaps,
        TouchesAnotherShip,
        InvalidLength,
    };

    /**
     * @brief Whose knowledge a board query is answered from.
     *
     * Owner sees their own ships; Opponent only sees the results of their attacks.
     * This is the fog-of-war rule, so it belongs to the model rather than the view.
     */
    enum class Visibility { Owner, Opponent };

    /**
     * @brief What is known to be at a cell. The view maps these to glyphs and colors.
     */
    enum class CellState {
        Water,
        Ship,
        Hit,
        Sunk,
        Miss,
    };
} // namespace cpp_warships::game_core
