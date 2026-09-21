#pragma once

#include <cstddef>
#include <functional>

namespace cpp_warships::game_core {
    /**
     * @brief A cell position on a board. Origin is the top-left corner.
     */
    struct Coordinate {
        int x = 0;
        int y = 0;

        friend bool operator==(const Coordinate& left, const Coordinate& right) = default;
    };

    /**
     * @brief Width of the coordinate space a single row is assumed to span.
     *
     * Any board this game supports is far narrower than this, which is what lets
     * flattenToSingleAxis give every cell its own number.
     */
    inline constexpr std::size_t COORDINATE_ROW_SPAN = 100003;

    /**
     * @brief Numbers the cells of the grid row by row, left to right.
     *
     * Reading the grid as one long line gives cell (x, y) the number
     * y * COORDINATE_ROW_SPAN + x. Two different cells only ever land on the same
     * number if a coordinate exceeds the row span, which no supported board does.
     */
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
