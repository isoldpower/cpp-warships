#include <game_persistence/serializers/BoardJsonSerializer.h>

#include <serialization/exceptions/DeserializationException.h>

#include <unordered_set>
#include <vector>

namespace cpp_warships::game_persistence {

    bool BoardJsonSerializer::isRelated(nlohmann::json item) {
        return item.contains("width") &&
            item.contains("height") &&
            item.contains("ships") &&
            item.contains("attackedCells");
    }

    nlohmann::json BoardJsonSerializer::serialize(game_core::Board& item) {
        ShipJsonSerializer shipSerializer = std::get<0>(childrenSerializers);

        nlohmann::json ships = nlohmann::json::array();
        for (game_core::Ship ship : item.ships()) {
            ships.push_back(shipSerializer.serialize(ship));
        }

        nlohmann::json attackedCells = nlohmann::json::array();
        for (const game_core::Coordinate& cell : item.attackedCells()) {
            attackedCells.push_back({
                {"x", cell.x},
                {"y", cell.y}
            });
        }

        return nlohmann::json{
                {"width", item.width()},
                {"height", item.height()},
                {"ships", ships},
                {"attackedCells", attackedCells}
        };
    }

    game_core::Board BoardJsonSerializer::deserialize(nlohmann::json item) {
        if (!isRelated(item)) {
            throw serialization::exceptions::DeserializationException(
                    "Board",
                    "JSON does not describe a board"
            );
        }

        ShipJsonSerializer shipSerializer = std::get<0>(childrenSerializers);
        std::vector<game_core::Ship> ships;
        for (const nlohmann::json& ship : item["ships"]) {
            ships.push_back(shipSerializer.deserialize(ship));
        }

        std::unordered_set<game_core::Coordinate> attackedCells;
        for (const nlohmann::json& cell : item["attackedCells"]) {
            attackedCells.insert({cell["x"].get<int>(), cell["y"].get<int>()});
        }

        return game_core::Board{
                item["width"].get<int>(),
                item["height"].get<int>(),
                std::move(ships),
                std::move(attackedCells)
        };
    }
} // namespace cpp_warships::game_persistence
