#include <game_persistence/serializers/SegmentJsonSerializer.h>

#include <serialization/exceptions/DeserializationException.h>

namespace cpp_warships::game_persistence {

    bool SegmentJsonSerializer::isRelated(nlohmann::json item) {
        return item.contains("maximumHealth") && item.contains("health");
    }

    nlohmann::json SegmentJsonSerializer::serialize(game_core::Segment& item) {
        return nlohmann::json{
            {"maximumHealth", item.maximumHealth()},
            {"health", item.health()}
        };
    }

    game_core::Segment SegmentJsonSerializer::deserialize(nlohmann::json item) {
        if (!isRelated(item)) {
            throw serialization::exceptions::DeserializationException(
                    "Segment",
                    "JSON does not describe a segment"
            );
        }

        return game_core::Segment{
            item["maximumHealth"].get<int>(),
            item["health"].get<int>()
        };
    }
} // namespace cpp_warships::game_persistence
