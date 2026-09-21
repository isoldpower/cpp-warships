#pragma once

#include <nlohmann/json.hpp>

#include <game_core/Segment.h>
#include <serialization/ISerializer.h>

namespace cpp_warships::game_persistence {
    inline char SEGMENT_SERIALIZER_NAME[] = "Segment";

    /** @brief Writes a segment as its maximum and current health. */
    class SegmentJsonSerializer final
        : public serialization::
                  ISerializer<nlohmann::json, game_core::Segment, SEGMENT_SERIALIZER_NAME> {
    public:
        bool isRelated(nlohmann::json item) override;
        nlohmann::json serialize(game_core::Segment& item) override;
        game_core::Segment deserialize(nlohmann::json item) override;
    };
} // namespace cpp_warships::game_persistence
