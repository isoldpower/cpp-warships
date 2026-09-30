#include <application/persistence/BrowserSaveStorage.h>

#include <cstdlib>
#include <sstream>
#include <utility>

extern "C" {
    char* warshipsStorageList(const char* keyPrefix);
    char* warshipsStorageRead(const char* key);
    int warshipsStorageWrite(const char* key, const char* contents);
    int warshipsStorageContains(const char* key);
    int warshipsStorageRemove(const char* key);
}

namespace cpp_warships::persistence {
    namespace {
        constexpr char NAME_SEPARATOR = '\n';

        /** @brief Adopts a string JavaScript allocated for us, freeing what it allocated. */
        [[nodiscard]] std::string adoptString(char* allocated) {
            if (allocated == nullptr) {
                return {};
            }

            std::string adopted{allocated};
            std::free(allocated);

            return adopted;
        }
    }  // namespace

    BrowserSaveStorage::BrowserSaveStorage(std::string keyPrefix)
        : keyPrefix_(std::move(keyPrefix)) {}

    const std::string& BrowserSaveStorage::keyPrefix() const noexcept {
        return keyPrefix_;
    }

    std::string BrowserSaveStorage::keyFor(const std::string& name) const {
        return keyPrefix_ + name;
    }

    std::vector<std::string> BrowserSaveStorage::list() const {
        const std::string joined = adoptString(warshipsStorageList(keyPrefix_.c_str()));

        std::vector<std::string> names;
        std::istringstream lines{joined};
        for (std::string name; std::getline(lines, name, NAME_SEPARATOR);) {
            if (!name.empty()) {
                names.push_back(name);
            }
        }

        return names;
    }

    std::optional<std::string> BrowserSaveStorage::read(const std::string& name) const {
        char* stored = warshipsStorageRead(keyFor(name).c_str());
        if (stored == nullptr) {
            return std::nullopt;
        }

        return adoptString(stored);
    }

    bool BrowserSaveStorage::write(const std::string& name, const std::string& contents) {
        return warshipsStorageWrite(keyFor(name).c_str(), contents.c_str()) != 0;
    }

    bool BrowserSaveStorage::contains(const std::string& name) const {
        return warshipsStorageContains(keyFor(name).c_str()) != 0;
    }

    bool BrowserSaveStorage::remove(const std::string& name) {
        return warshipsStorageRemove(keyFor(name).c_str()) != 0;
    }
}  // namespace cpp_warships::persistence
