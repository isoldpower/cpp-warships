#include <game_persistence/serializers/MatchSettingsJsonSerializer.h>

#include <serialization/exceptions/DeserializationException.h>

#include <map>

namespace cpp_warships::game_persistence {

    bool MatchSettingsJsonSerializer::isRelated(nlohmann::json item) {
        return item.contains("boardSize") &&
            item.contains("fleet") &&
            item.contains("baseDamage") &&
            item.contains("segmentHealth");
    }

    nlohmann::json MatchSettingsJsonSerializer::serialize(game_core::MatchSettings& item) {
        nlohmann::json fleet = nlohmann::json::array();
        for (const auto& [length, count] : item.fleet().countsByLength()) {
            fleet.push_back({{"length", length}, {"count", count}});
        }

        return nlohmann::json{
                {"boardSize", item.boardSize()},
                {"fleet", fleet},
                {"baseDamage", item.baseDamage()},
                {"segmentHealth", item.segmentHealth()}
        };
    }

    game_core::MatchSettings MatchSettingsJsonSerializer::deserialize(nlohmann::json item) {
        if (!isRelated(item)) {
            throw serialization::exceptions::DeserializationException(
                    "MatchSettings",
                    "JSON does not describe match settings"
            );
        }

        std::map<int, int> countsByLength;
        for (const nlohmann::json& entry : item["fleet"]) {
            countsByLength[entry["length"].get<int>()] = entry["count"].get<int>();
        }

        return game_core::MatchSettings{
                item["boardSize"].get<int>(),
                game_core::FleetComposition{std::move(countsByLength)},
                item["baseDamage"].get<int>(),
                item["segmentHealth"].get<int>()
        };
    }
} // namespace cpp_warships::game_persistence
