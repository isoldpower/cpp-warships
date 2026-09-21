#pragma once

#include <map>
#include <string>

#include <game_persistence/SaveStorage.h>

namespace cpp_warships::game_persistence {
    /** @brief Keeps saves in memory for the lifetime of the object.
     *  Used by tests, and the shape a browser-backed storage would take. */
    class MemorySaveStorage final : public SaveStorage {
    public:
        [[nodiscard]] std::vector<std::string> list() const override;
        [[nodiscard]] std::optional<std::string> read(const std::string& name) const override;
        bool write(const std::string& name, const std::string& contents) override;
        [[nodiscard]] bool contains(const std::string& name) const override;

    private:
        std::map<std::string, std::string> savesByName_;
    };
} // namespace cpp_warships::game_persistence
