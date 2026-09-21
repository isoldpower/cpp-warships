#include <game_persistence/MemorySaveStorage.h>

namespace cpp_warships::game_persistence {

    std::vector<std::string> MemorySaveStorage::list() const {
        std::vector<std::string> names;
        names.reserve(savesByName_.size());
        for (const auto& [name, contents] : savesByName_) {
            names.push_back(name);
        }

        return names;
    }

    std::optional<std::string> MemorySaveStorage::read(const std::string& name) const {
        const auto found = savesByName_.find(name);
        std::optional<std::string> contents;

        if (found != savesByName_.end()) {
            contents = found->second;
        }

        return contents;
    }

    bool MemorySaveStorage::write(const std::string& name, const std::string& contents) {
        savesByName_[name] = contents;
        return true;
    }

    bool MemorySaveStorage::contains(const std::string& name) const {
        return savesByName_.contains(name);
    }
} // namespace cpp_warships::game_persistence
