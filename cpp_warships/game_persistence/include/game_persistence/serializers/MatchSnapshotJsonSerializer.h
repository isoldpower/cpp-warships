#pragma once

#include <nlohmann/json.hpp>

#include <game_persistence/MatchSnapshot.h>
#include <game_persistence/serializers/BoardJsonSerializer.h>
#include <game_persistence/serializers/MatchSettingsJsonSerializer.h>
#include <serialization/ISerializer.h>

namespace cpp_warships::game_persistence {
    /** @brief Writes a whole match: its settings, both boards and where play had got to. */
    class MatchSnapshotJsonSerializer final : public serialization::ISerializer<
                                                      nlohmann::json,
                                                      MatchSnapshot,
                                                      MATCH_SNAPSHOT_NAME,
                                                      BoardJsonSerializer,
                                                      MatchSettingsJsonSerializer> {
    public:
        bool isRelated(nlohmann::json item) override;
        nlohmann::json serialize(MatchSnapshot& item) override;
        MatchSnapshot deserialize(nlohmann::json item) override;
    };
} // namespace cpp_warships::game_persistence
