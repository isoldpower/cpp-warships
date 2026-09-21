#pragma once

#include <nlohmann/json.hpp>

#include <game_core/MatchSettings.h>
#include <serialization/ISerializer.h>

namespace cpp_warships::game_persistence {
    inline char MATCH_SETTINGS_SERIALIZER_NAME[] = "MatchSettings";

    /** @brief Writes the rules a match is played under, fleet composition included. */
    class MatchSettingsJsonSerializer final : public serialization::ISerializer<
                                                      nlohmann::json,
                                                      game_core::MatchSettings,
                                                      MATCH_SETTINGS_SERIALIZER_NAME> {
    public:
        bool isRelated(nlohmann::json item) override;
        nlohmann::json serialize(game_core::MatchSettings& item) override;
        game_core::MatchSettings deserialize(nlohmann::json item) override;
    };
} // namespace cpp_warships::game_persistence
