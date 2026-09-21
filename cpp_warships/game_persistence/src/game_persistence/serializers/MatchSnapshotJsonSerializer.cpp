#include <game_persistence/serializers/MatchSnapshotJsonSerializer.h>

#include <serialization/exceptions/DeserializationException.h>

#include <algorithm>
#include <deque>
#include <string>
#include <unordered_map>
#include <utility>

namespace cpp_warships::game_persistence {
    namespace {
        const std::unordered_map<game_flow::SkillKind, std::string> NAME_BY_SKILL{
                {game_flow::SkillKind::Scanner, "scanner"},
                {game_flow::SkillKind::DoubleDamage, "doubleDamage"},
                {game_flow::SkillKind::RandomStrike, "randomStrike"}
        };

        const std::unordered_map<game_flow::MatchPhase, std::string> NAME_BY_PHASE{
                {game_flow::MatchPhase::Placement, "placement"},
                {game_flow::MatchPhase::Battle, "battle"},
                {game_flow::MatchPhase::Finished, "finished"}
        };

        const std::unordered_map<game_flow::Participant, std::string> NAME_BY_PARTICIPANT{
                {game_flow::Participant::Player, "player"},
                {game_flow::Participant::Computer, "computer"}
        };

        template <typename TValue>
        TValue valueForName(
                const std::unordered_map<TValue, std::string>& namesByValue,
                const std::string& name,
                const std::string& what
        ) {
            const auto matchesName = [&name](const std::pair<const TValue, std::string>& entry) {
                return entry.second == name;
            };

            const auto found = std::find_if(
                namesByValue.begin(),
                namesByValue.end(),
                matchesName
            );
            if (found == namesByValue.end()) {
                throw serialization::exceptions::DeserializationException(
                        what,
                        "unknown name: " + name
                );
            }

            return found->first;
        }
    } // namespace

    bool MatchSnapshotJsonSerializer::isRelated(nlohmann::json item) {
        return item.contains("settings") && item.contains("playerBoard") &&
               item.contains("computerBoard") && item.contains("phase");
    }

    nlohmann::json MatchSnapshotJsonSerializer::serialize(MatchSnapshot& item) {
        BoardJsonSerializer boardSerializer = std::get<0>(childrenSerializers);
        MatchSettingsJsonSerializer settingsSerializer = std::get<1>(childrenSerializers);

        game_core::Board playerBoard = item.playerBoard();
        game_core::Board computerBoard = item.computerBoard();
        game_core::MatchSettings settings = item.settings();

        nlohmann::json bankedSkills = nlohmann::json::array();
        for (const game_flow::SkillKind skill : item.bankedSkills()) {
            bankedSkills.push_back(NAME_BY_SKILL.at(skill));
        }

        return nlohmann::json{
                {"settings", settingsSerializer.serialize(settings)},
                {"playerBoard", boardSerializer.serialize(playerBoard)},
                {"computerBoard", boardSerializer.serialize(computerBoard)},
                {"bankedSkills", bankedSkills},
                {"roundNumber", item.roundNumber()},
                {"phase", NAME_BY_PHASE.at(item.phase())},
                {"currentTurn", NAME_BY_PARTICIPANT.at(item.currentTurn())},
                {"isDoubleDamageArmed", item.isDoubleDamageArmed()}
        };
    }

    MatchSnapshot MatchSnapshotJsonSerializer::deserialize(nlohmann::json item) {
        if (!isRelated(item)) {
            throw serialization::exceptions::DeserializationException(
                    "MatchSnapshot",
                    "JSON does not describe a match snapshot"
            );
        }

        BoardJsonSerializer boardSerializer = std::get<0>(childrenSerializers);
        MatchSettingsJsonSerializer settingsSerializer = std::get<1>(childrenSerializers);

        std::deque<game_flow::SkillKind> bankedSkills;
        for (const nlohmann::json& skill : item["bankedSkills"]) {
            bankedSkills.push_back(
                    valueForName(NAME_BY_SKILL, skill.get<std::string>(), "SkillKind")
            );
        }

        const game_flow::MatchPhase phase =
                valueForName(NAME_BY_PHASE, item["phase"].get<std::string>(), "MatchPhase");
        const game_flow::Participant currentTurn = valueForName(
                NAME_BY_PARTICIPANT,
                item["currentTurn"].get<std::string>(),
                "Participant"
        );

        return MatchSnapshot{
                settingsSerializer.deserialize(item["settings"]),
                boardSerializer.deserialize(item["playerBoard"]),
                boardSerializer.deserialize(item["computerBoard"]),
                std::move(bankedSkills),
                item["roundNumber"].get<int>(),
                phase,
                currentTurn,
                item["isDoubleDamageArmed"].get<bool>()
        };
    }
} // namespace cpp_warships::game_persistence
