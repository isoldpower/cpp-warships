#include <application/persistence/serializers/MatchSnapshotJsonSerializer.h>
#include <serialization/exceptions/DeserializationException.h>

#include <algorithm>
#include <deque>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace cpp_warships::persistence::serializers {
    namespace {
        const std::unordered_map<flow::SkillKind, std::string> NAME_BY_SKILL{
            {flow::SkillKind::Scanner, "scanner"},
            {flow::SkillKind::DoubleDamage, "doubleDamage"},
            {flow::SkillKind::RandomStrike, "randomStrike"}
        };

        const std::unordered_map<flow::MatchPhase, std::string> NAME_BY_PHASE{
            {flow::MatchPhase::Placement, "placement"},
            {flow::MatchPhase::Battle, "battle"},
            {flow::MatchPhase::Finished, "finished"}
        };

        const std::unordered_map<flow::MatchEventKind, std::string> NAME_BY_EVENT_KIND{
            {flow::MatchEventKind::ShotMissed, "shotMissed"},
            {flow::MatchEventKind::ShipDamaged, "shipDamaged"},
            {flow::MatchEventKind::ShipSunk, "shipSunk"},
            {flow::MatchEventKind::ShotRejected, "shotRejected"},
            {flow::MatchEventKind::SkillGranted, "skillGranted"},
            {flow::MatchEventKind::DoubleDamageArmed, "doubleDamageArmed"},
            {flow::MatchEventKind::AreaScanned, "areaScanned"},
            {flow::MatchEventKind::RoundWon, "roundWon"},
            {flow::MatchEventKind::MatchLost, "matchLost"},
            {flow::MatchEventKind::TurnPassed, "turnPassed"}
        };

        const std::unordered_map<flow::Participant, std::string> NAME_BY_PARTICIPANT{
            {flow::Participant::Player, "player"},
            {flow::Participant::Computer, "computer"}
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

            const auto found = std::find_if(namesByValue.begin(), namesByValue.end(), matchesName);
            if (found == namesByValue.end()) {
                throw serialization::exceptions::DeserializationException(
                    what,
                    "unknown name: " + name
                );
            }

            return found->first;
        }
        [[nodiscard]] nlohmann::json coordinatesToJson(
            const std::vector<core::Coordinate>& coordinates
        ) {
            nlohmann::json written = nlohmann::json::array();
            for (const core::Coordinate& coordinate : coordinates) {
                written.push_back({{"x", coordinate.x}, {"y", coordinate.y}});
            }

            return written;
        }

        [[nodiscard]] std::vector<core::Coordinate> coordinatesFromJson(
            const nlohmann::json& written
        ) {
            std::vector<core::Coordinate> coordinates;
            for (const nlohmann::json& coordinate : written) {
                coordinates.push_back({coordinate["x"].get<int>(), coordinate["y"].get<int>()});
            }

            return coordinates;
        }

        /** @brief What the computer knows, written out so a loaded game keeps a
         * sharp enemy. */
        [[nodiscard]] nlohmann::json memoryToJson(const flow::AiMemory& memory) {
            const std::vector<core::Coordinate> attempted{
                memory.attemptedCoordinates.begin(),
                memory.attemptedCoordinates.end()
            };

            return nlohmann::json{
                {"attemptedCoordinates", coordinatesToJson(attempted)},
                {"currentTargetHits", coordinatesToJson(memory.currentTargetHits)}
            };
        }

        /** @brief The computer's knowledge read back, empty for a save written before it was
         * kept, which simply means that enemy starts the load looking again. */
        [[nodiscard]] flow::AiMemory memoryFromJson(const nlohmann::json& item) {
            if (!item.contains("opponentMemory")) {
                return {};
            }

            const nlohmann::json& memory = item["opponentMemory"];
            const std::vector<core::Coordinate> attempted =
                coordinatesFromJson(memory["attemptedCoordinates"]);

            return flow::AiMemory{
                .attemptedCoordinates = {attempted.begin(), attempted.end()},
                .currentTargetHits = coordinatesFromJson(memory["currentTargetHits"])
            };
        }
        /** @brief The story so far, written out so that a loaded game reads
         * back with the log it had rather than starting silent. */
        [[nodiscard]] nlohmann::json journalToJson(const flow::MatchEventLog& journal) {
            nlohmann::json written = nlohmann::json::array();
            for (const flow::MatchEvent& event : journal) {
                nlohmann::json entry{
                    {"kind", NAME_BY_EVENT_KIND.at(event.kind)},
                    {"actor", NAME_BY_PARTICIPANT.at(event.actor)},
                    {"scanFoundShip", event.scanFoundShip}
                };
                if (event.coordinate.has_value()) {
                    entry["coordinate"] = {{"x", event.coordinate->x}, {"y", event.coordinate->y}};
                }
                if (event.skill.has_value()) {
                    entry["skill"] = NAME_BY_SKILL.at(*event.skill);
                }

                written.push_back(std::move(entry));
            }

            return written;
        }

        /** @brief The story read back, empty for a save written before it was kept, which simply
         * means that game carries on with nothing behind it. */
        [[nodiscard]] flow::MatchEventLog journalFromJson(const nlohmann::json& item) {
            if (!item.contains("journal")) {
                return {};
            }

            flow::MatchEventLog journal;
            for (const nlohmann::json& entry : item["journal"]) {
                flow::MatchEvent event{
                    .kind = valueForName(
                        NAME_BY_EVENT_KIND,
                        entry["kind"].get<std::string>(),
                        "MatchEventKind"
                    ),
                    .actor = valueForName(
                        NAME_BY_PARTICIPANT,
                        entry["actor"].get<std::string>(),
                        "Participant"
                    ),
                    .scanFoundShip = entry.value("scanFoundShip", false)
                };
                if (entry.contains("coordinate")) {
                    event.coordinate = core::Coordinate{
                        entry["coordinate"]["x"].get<int>(),
                        entry["coordinate"]["y"].get<int>()
                    };
                }
                if (entry.contains("skill")) {
                    event.skill =
                        valueForName(NAME_BY_SKILL, entry["skill"].get<std::string>(), "SkillKind");
                }

                journal.push_back(event);
            }

            return journal;
        }

        /** @brief The banked skills a save lists by name, in the order they will be spent. */
        std::deque<flow::SkillKind> skillsFromJson(const nlohmann::json& names) {
            std::deque<flow::SkillKind> skills;
            for (const nlohmann::json& name : names) {
                skills.push_back(valueForName(NAME_BY_SKILL, name.get<std::string>(), "SkillKind"));
            }

            return skills;
        }
    }  // namespace

    bool MatchSnapshotJsonSerializer::isRelated(nlohmann::json item) {
        return item.contains("settings") && item.contains("playerBoard") &&
               item.contains("computerBoard") && item.contains("phase");
    }

    nlohmann::json MatchSnapshotJsonSerializer::serialize(MatchSnapshot& item) {
        BoardJsonSerializer boardSerializer = std::get<0>(childrenSerializers);
        MatchSettingsJsonSerializer settingsSerializer = std::get<1>(childrenSerializers);

        core::Board playerBoard = item.playerBoard();
        core::Board computerBoard = item.computerBoard();
        core::MatchSettings settings = item.settings();

        nlohmann::json bankedSkills = nlohmann::json::array();
        for (const flow::SkillKind skill : item.bankedSkills()) {
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
            {"isDoubleDamageArmed", item.isDoubleDamageArmed()},
            {"opponentMemory", memoryToJson(item.opponentMemory())},
            {"journal", journalToJson(item.journal())}
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

        const flow::MatchPhase phase =
            valueForName(NAME_BY_PHASE, item["phase"].get<std::string>(), "MatchPhase");
        const flow::Participant currentTurn = valueForName(
            NAME_BY_PARTICIPANT,
            item["currentTurn"].get<std::string>(),
            "Participant"
        );

        return MatchSnapshot{
            settingsSerializer.deserialize(item["settings"]),
            boardSerializer.deserialize(item["playerBoard"]),
            boardSerializer.deserialize(item["computerBoard"]),
            skillsFromJson(item["bankedSkills"]),
            item["roundNumber"].get<int>(),
            phase,
            currentTurn,
            item["isDoubleDamageArmed"].get<bool>(),
            memoryFromJson(item),
            journalFromJson(item)
        };
    }
}  // namespace cpp_warships::persistence::serializers
