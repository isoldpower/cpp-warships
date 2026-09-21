#pragma once

#include <cstddef>
#include <functional>

namespace cpp_warships::game_core {
    /** @brief A cell position on a board. Origin is the top-left corner. */
    struct Coordinate {
        int x = 0;
        int y = 0;

        friend bool operator==(const Coordinate& left, const Coordinate& right) = default;
    };

    /** @brief Width of coordinate space a row spans; every supported board is far narrower.
     *  That margin is what lets flattenToSingleAxis give each cell its own number. */
    inline constexpr std::size_t COORDINATE_ROW_SPAN = 100003;

    /** @brief Numbers cells row by row: cell (x, y) becomes y * COORDINATE_ROW_SPAN + x.
     *  Distinct cells collide only past the row span, which no supported board reaches. */
    inline std::size_t flattenToSingleAxis(const Coordinate& coordinate) noexcept {
        const auto row = static_cast<std::size_t>(coordinate.y);
        const auto column = static_cast<std::size_t>(coordinate.x);
        return row * COORDINATE_ROW_SPAN + column;
    }
} // namespace cpp_warships::game_core

template <>
struct std::hash<cpp_warships::game_core::Coordinate> {
    std::size_t operator()(const cpp_warships::game_core::Coordinate& coordinate) const noexcept {
        return cpp_warships::game_core::flattenToSingleAxis(coordinate);
    }
};
