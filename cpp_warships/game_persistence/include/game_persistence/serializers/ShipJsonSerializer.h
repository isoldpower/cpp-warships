#pragma once

#include <nlohmann/json.hpp>

#include <game_core/Ship.h>
#include <game_persistence/serializers/SegmentJsonSerializer.h>
#include <serialization/ISerializer.h>

namespace cpp_warships::game_persistence {
    inline char SHIP_SERIALIZER_NAME[] = "Ship";

    /** @brief Writes a ship as its origin, orientation and segments.
     *  Coordinates are derived on load, so only the origin needs storing. */
    class ShipJsonSerializer final : public serialization::ISerializer<
                                             nlohmann::json,
                                             game_core::Ship,
                                             SHIP_SERIALIZER_NAME,
                                             SegmentJsonSerializer> {
    public:
        bool isRelated(nlohmann::json item) override;
        nlohmann::json serialize(game_core::Ship& item) override;
        game_core::Ship deserialize(nlohmann::json item) override;
    };
} // namespace cpp_warships::game_persistence
