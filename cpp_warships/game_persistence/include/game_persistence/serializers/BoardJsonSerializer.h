#pragma once

#include <nlohmann/json.hpp>

#include <game_core/Board.h>
#include <game_persistence/serializers/ShipJsonSerializer.h>
#include <serialization/ISerializer.h>

namespace cpp_warships::game_persistence {
    inline char BOARD_SERIALIZER_NAME[] = "Board";

    /** @brief Writes a board as its size, its ships and the cells attacked so far. */
    class BoardJsonSerializer final : public serialization::ISerializer<
                                              nlohmann::json,
                                              game_core::Board,
                                              BOARD_SERIALIZER_NAME,
                                              ShipJsonSerializer> {
    public:
        bool isRelated(nlohmann::json item) override;
        nlohmann::json serialize(game_core::Board& item) override;
        game_core::Board deserialize(nlohmann::json item) override;
    };
} // namespace cpp_warships::game_persistence
