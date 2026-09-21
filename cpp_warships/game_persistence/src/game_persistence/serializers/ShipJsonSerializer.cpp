#include <game_persistence/serializers/ShipJsonSerializer.h>
#include <serialization/exceptions/DeserializationException.h>

#include <vector>

namespace cpp_warships::game_persistence {
    namespace {
        constexpr const char* HORIZONTAL_DIRECTION = "horizontal";
        constexpr const char* VERTICAL_DIRECTION = "vertical";
    } // namespace

    bool ShipJsonSerializer::isRelated(nlohmann::json item) {
        return item.contains("origin") &&
            item.contains("direction") &&
            item.contains("segments");
    }

    nlohmann::json ShipJsonSerializer::serialize(game_core::Ship& item) {
        SegmentJsonSerializer segmentSerializer = std::get<0>(childrenSerializers);

        nlohmann::json segments = nlohmann::json::array();
        for (game_core::Segment segment : item.segments()) {
            segments.push_back(segmentSerializer.serialize(segment));
        }

        const bool isHorizontal = item.direction() == game_core::Direction::Horizontal;
        return nlohmann::json{
                {"origin", {
                    {"x", item.origin().x},
                    {"y", item.origin().y}
                }},
                {"direction", isHorizontal ? HORIZONTAL_DIRECTION : VERTICAL_DIRECTION},
                {"segments", segments}
        };
    }

    game_core::Ship ShipJsonSerializer::deserialize(nlohmann::json item) {
        if (!isRelated(item)) {
            throw serialization::exceptions::DeserializationException(
                    "Ship",
                    "JSON does not describe a ship"
            );
        }

        SegmentJsonSerializer segmentSerializer = std::get<0>(childrenSerializers);

        std::vector<game_core::Segment> segments;
        for (const nlohmann::json& segment : item["segments"]) {
            segments.push_back(segmentSerializer.deserialize(segment));
        }

        const game_core::Coordinate origin{
                item["origin"]["x"].get<int>(),
                item["origin"]["y"].get<int>()
        };
        const bool isHorizontal = item["direction"].get<std::string>() == HORIZONTAL_DIRECTION;
        const game_core::Direction direction = isHorizontal ? game_core::Direction::Horizontal
                                                            : game_core::Direction::Vertical;

        return game_core::Ship{origin, direction, std::move(segments)};
    }
} // namespace cpp_warships::game_persistence
