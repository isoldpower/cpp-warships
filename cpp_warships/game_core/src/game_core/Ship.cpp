#include <game_core/Ship.h>

#include <algorithm>
#include <cstddef>
#include <optional>
#include <stdexcept>

namespace cpp_warships::game_core {

    Ship::Ship(Coordinate origin, Direction direction, int length, int segmentHealth)
        : origin_(origin)
        , direction_(direction) {
        segments_.reserve(static_cast<std::size_t>(std::max(0, length)));

        for (int index = 0; index < length; ++index) {
            segments_.emplace_back(segmentHealth);
        }
    }

    Coordinate Ship::origin() const noexcept {
        return origin_;
    }

    Direction Ship::direction() const noexcept {
        return direction_;
    }

    int Ship::length() const noexcept {
        return static_cast<int>(segments_.size());
    }

    Coordinate Ship::coordinateAt(int index) const {
        if (index < 0 || index >= length()) {
            throw std::out_of_range("Ship::coordinateAt: segment index out of range");
        }

        return direction_ == Direction::Horizontal ? Coordinate{origin_.x + index, origin_.y}
                                                   : Coordinate{origin_.x, origin_.y + index};
    }

    std::optional<int> Ship::segmentIndexAt(Coordinate coordinate) const noexcept {
        const int offset = direction_ == Direction::Horizontal ? coordinate.x - origin_.x
                                                               : coordinate.y - origin_.y;
        const bool isOnAxis = direction_ == Direction::Horizontal ? coordinate.y == origin_.y
                                                                  : coordinate.x == origin_.x;

        if (!isOnAxis || offset < 0 || offset >= length()) {
            return std::nullopt;
        } else {
            return offset;
        }
    }

    bool Ship::occupies(Coordinate coordinate) const noexcept {
        return segmentIndexAt(coordinate).has_value();
    }

    std::vector<Coordinate> Ship::coordinates() const {
        std::vector<Coordinate> result;
        result.reserve(segments_.size());
        for (int index = 0; index < length(); ++index) {
            result.push_back(coordinateAt(index));
        }

        return result;
    }

    bool Ship::damageSegment(int index, int amount) {
        if (index < 0 || index >= length()) {
            return false;
        }

        segments_[static_cast<std::size_t>(index)].takeDamage(amount);
        return true;
    }

    int Ship::segmentHealth(int index) const {
        if (index < 0 || index >= length()) {
            throw std::out_of_range("Ship::segmentHealth: segment index out of range");
        }

        return segments_[static_cast<std::size_t>(index)].health();
    }

    bool Ship::isSunk() const noexcept {
        const auto isSegmentDestroyed = [](const Segment& segment) {
            return segment.isDestroyed();
        };

        return std::all_of(
            segments_.begin(),
            segments_.end(),
            isSegmentDestroyed
        );
    }

    const std::vector<Segment>& Ship::segments() const noexcept {
        return segments_;
    }
} // namespace cpp_warships::game_core
