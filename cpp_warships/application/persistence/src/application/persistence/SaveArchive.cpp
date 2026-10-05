#include <application/persistence/SaveArchive.h>
#include <application/persistence/SaveStorage.h>
#include <application/persistence/serializers/MatchSnapshotJsonSerializer.h>
#include <platform/OSBrancher.h>

#include <algorithm>
#include <chrono>
#include <ctime>
#include <exception>
#include <iomanip>
#include <map>
#include <nlohmann/json.hpp>
#include <random>
#include <sstream>

namespace cpp_warships::persistence {
    namespace {
        /** @brief How many spaces a written save is indented by, to stay
         * readable. */
        constexpr int SAVE_INDENTATION = 4;

        constexpr const char* DATA_KEY = "data";
        constexpr const char* META_KEY = "meta";
        constexpr const char* UUID_KEY = "uuid";
        constexpr const char* TIMESTAMP_KEY = "timestamp";
        constexpr const char* NAME_KEY = "name";

        /** @brief Length of a timestamp written as YYYY-MM-DDTHH:MM:SS. */
        constexpr std::size_t TIMESTAMP_LENGTH = 19;
        constexpr std::size_t TIMESTAMP_SEPARATOR_POSITION = 10;

        /** @brief Length of the identifier saves carried before they were wrapped in a
         * meta block, written as YYYYMMDD-HHMMSS. */
        constexpr std::size_t LEGACY_ID_LENGTH = 15;
        constexpr std::size_t LEGACY_ID_SEPARATOR_POSITION = 8;

        /** @brief The moment a save of the older shape was made, read out of its identifier
         * because it kept no timestamp of its own. */
        [[nodiscard]] std::string timestampOfLegacyId(const std::string& id) {
            if (id.size() != LEGACY_ID_LENGTH || id[LEGACY_ID_SEPARATOR_POSITION] != '-') {
                return {};
            }

            return id.substr(0, 4) + "-" + id.substr(4, 2) + "-" + id.substr(6, 2) + "T" +
                   id.substr(9, 2) + ":" + id.substr(11, 2) + ":" + id.substr(13, 2);
        }

        /** @brief A snapshot serializer with its children wired up. */
        serializers::MatchSnapshotJsonSerializer makeSnapshotSerializer() {
            serializers::MatchSnapshotJsonSerializer serializer;
            serializers::BoardJsonSerializer boardSerializer;
            serializers::MatchSettingsJsonSerializer settingsSerializer;
            serializers::ShipJsonSerializer shipSerializer;
            serializers::SegmentJsonSerializer segmentSerializer;

            shipSerializer.setChildrenSerializers(&segmentSerializer);
            boardSerializer.setChildrenSerializers(&shipSerializer);
            serializer.setChildrenSerializers(&boardSerializer, &settingsSerializer);

            return serializer;
        }

        /** @brief The meta block an older save never wrote, rebuilt from the name it kept beside
         * the match and the time its id was made at. */
        [[nodiscard]] std::optional<nlohmann::json> legacyMetaOf(
            const nlohmann::json& document,
            const std::string& id
        ) {
            if (!document.contains(NAME_KEY)) {
                return std::nullopt;
            }

            return nlohmann::json{
                {UUID_KEY, id},
                {TIMESTAMP_KEY, timestampOfLegacyId(id)},
                {NAME_KEY, document[NAME_KEY]}
            };
        }

        /** @brief The match a save holds: under its data block, or at the top level of an older
         * save that had no blocks at all. */
        [[nodiscard]] const nlohmann::json& matchDocumentOf(const nlohmann::json& document) {
            return document.contains(DATA_KEY) ? document.at(DATA_KEY) : document;
        }

        /** @brief The meta block of the save called @p id, or nothing when it
         * cannot be read. */
        [[nodiscard]] std::optional<nlohmann::json> metaOf(
            const SaveStorage& storage,
            const std::string& id
        ) {
            const std::optional<std::string> contents = storage.read(id);
            if (!contents.has_value()) {
                return std::nullopt;
            }

            try {
                const nlohmann::json document = nlohmann::json::parse(*contents);
                if (document.contains(META_KEY) && document[META_KEY].is_object()) {
                    return document[META_KEY];
                }

                return legacyMetaOf(document, id);
            } catch (const std::exception&) {
                return std::nullopt;
            }
        }

        /** @brief Reads @p key of @p meta as text, or nothing when it is not there. */
        [[nodiscard]] std::optional<std::string> textOf(
            const std::optional<nlohmann::json>& meta,
            const char* key
        ) {
            if (!meta.has_value() || !meta->contains(key) || !(*meta)[key].is_string()) {
                return std::nullopt;
            }

            return (*meta)[key].get<std::string>();
        }
    }  // namespace

    SaveArchive::SaveArchive(SaveStorage& storage)
        : storage_(storage) {}

    std::vector<SaveSummary> SaveArchive::listSaves() const {
        std::vector<SaveSummary> summaries;
        for (const std::string& id : storage_.list()) {
            const std::optional<nlohmann::json> meta = metaOf(storage_, id);

            summaries.push_back(
                {.id = id,
                 .name = textOf(meta, NAME_KEY).value_or(id),
                 .timestamp = textOf(meta, TIMESTAMP_KEY).value_or(std::string{})}
            );
        }

        const auto isNewerFirst = [](const SaveSummary& left, const SaveSummary& right) {
            if (left.timestamp != right.timestamp) {
                return left.timestamp > right.timestamp;
            }

            return left.id < right.id;
        };
        std::sort(summaries.begin(), summaries.end(), isNewerFirst);

        return summaries;
    }

    std::optional<std::string> SaveArchive::nameOf(const std::string& id) const {
        return textOf(metaOf(storage_, id), NAME_KEY);
    }

    std::string SaveArchive::newSaveId() {
        static constexpr const char* HEX_DIGITS = "0123456789abcdef";
        static constexpr int UUID_VARIANT_LOW = 8;
        static constexpr int UUID_VARIANT_HIGH = 11;

        std::random_device randomDevice;
        std::mt19937 engine{randomDevice()};
        std::uniform_int_distribution<int> anyDigit{0, 15};
        std::uniform_int_distribution<int> variantDigit{UUID_VARIANT_LOW, UUID_VARIANT_HIGH};

        std::map<char, std::uniform_int_distribution<int>*> digitsByPlaceholder{
            {'x', &anyDigit},
            {'y', &variantDigit},
        };

        std::string written = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
        for (char& position : written) {
            const auto digits = digitsByPlaceholder.find(position);
            if (digits != digitsByPlaceholder.end()) {
                position = HEX_DIGITS[(*digits->second)(engine)];
            }
        }

        return written;
    }

    std::string SaveArchive::timestampForNow() {
        const std::time_t now =
            std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        const std::tm broken = platform::localTimeOf(now);

        std::ostringstream written;
        written << std::put_time(&broken, "%Y-%m-%dT%H:%M:%S");

        return written.str();
    }

    std::string SaveArchive::momentOf(const std::string& timestamp) {
        if (timestamp.size() != TIMESTAMP_LENGTH ||
            timestamp[TIMESTAMP_SEPARATOR_POSITION] != 'T') {
            return timestamp;
        }

        return timestamp.substr(0, TIMESTAMP_SEPARATOR_POSITION) + "  " +
               timestamp.substr(TIMESTAMP_SEPARATOR_POSITION + 1);
    }

    bool SaveArchive::remove(const std::string& id) {
        return storage_.remove(id);
    }

    bool SaveArchive::save(
        const std::string& id,
        const std::string& name,
        const MatchSnapshot& snapshot
    ) {
        serializers::MatchSnapshotJsonSerializer serializer = makeSnapshotSerializer();
        MatchSnapshot writableSnapshot = snapshot;
        bool isSaved = false;

        try {
            const nlohmann::json document{
                {DATA_KEY, serializer.serialize(writableSnapshot)},
                {META_KEY, {{UUID_KEY, id}, {TIMESTAMP_KEY, timestampForNow()}, {NAME_KEY, name}}}
            };
            isSaved = storage_.write(id, document.dump(SAVE_INDENTATION));
        } catch (const std::exception&) {
            isSaved = false;
        }

        return isSaved;
    }

    std::optional<MatchSnapshot> SaveArchive::load(const std::string& id) const {
        const std::optional<std::string> contents = storage_.read(id);
        std::optional<MatchSnapshot> snapshot;

        if (contents.has_value()) {
            serializers::MatchSnapshotJsonSerializer serializer = makeSnapshotSerializer();
            try {
                const nlohmann::json document = nlohmann::json::parse(*contents);
                snapshot = serializer.deserialize(matchDocumentOf(document));
            } catch (const std::exception&) {
                snapshot = std::nullopt;
            }
        }

        return snapshot;
    }
}  // namespace cpp_warships::persistence
