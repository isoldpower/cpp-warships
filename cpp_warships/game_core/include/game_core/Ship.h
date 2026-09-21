#pragma once

#include <optional>
#include <vector>

#include <game_core/Coordinate.h>
#include <game_core/Direction.h>
#include <game_core/Segment.h>

namespace cpp_warships::game_core {
    /**
     * @brief A placed ship: its origin, orientation and per-cell segments.
     *
     * The ship derives its occupied coordinates from origin and direction, so segment
     * index and coordinate always agree.
     */
    class Ship {
    public:
        Ship(Coordinate origin,
             Direction direction,
             int length,
             int segmentHealth = DEFAULT_SEGMENT_HEALTH);

        [[nodiscard]] Coordinate origin() const noexcept;
        [[nodiscard]] Direction direction() const noexcept;
        [[nodiscard]] int length() const noexcept;

        /** @brief Coordinate of the segment at @p index, which must be in range. */
        [[nodiscard]] Coordinate coordinateAt(int index) const;

        /** @brief Index of the segment covering @p coordinate, or nullopt when not covered. */
        [[nodiscard]] std::optional<int> segmentIndexAt(Coordinate coordinate) const noexcept;

        [[nodiscard]] bool occupies(Coordinate coordinate) const noexcept;
        [[nodiscard]] std::vector<Coordinate> coordinates() const;

        /** @brief Damages the segment at @p index; returns false when out of range. */
        bool damageSegment(int index, int amount);

        [[nodiscard]] int segmentHealth(int index) const;
        [[nodiscard]] bool isSunk() const noexcept;
        [[nodiscard]] const std::vector<Segment>& segments() const noexcept;

    private:
        Coordinate origin_;
        Direction direction_;
        std::vector<Segment> segments_;
    };
} // namespace cpp_warships::game_core
