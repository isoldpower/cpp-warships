#pragma once

#include <optional>
#include <string>
#include <vector>

namespace cpp_warships::game_persistence {
    /** @brief Where saved games are kept, named but otherwise opaque to the caller.
     *  Only an implementation of this knows about files, so nothing above it needs to. */
    class SaveStorage {
    public:
        virtual ~SaveStorage() = default;

        /** @brief Names of every save currently held. */
        [[nodiscard]] virtual std::vector<std::string> list() const = 0;

        /** @brief Reads the save called @p name, or nullopt when there is none. */
        [[nodiscard]] virtual std::optional<std::string> read(const std::string& name) const = 0;

        /** @brief Writes @p contents under @p name, replacing any save already there.
         *  @return false when the save could not be written. */
        virtual bool write(const std::string& name, const std::string& contents) = 0;

        /** @brief Whether a save called @p name exists. */
        [[nodiscard]] virtual bool contains(const std::string& name) const = 0;
    };
} // namespace cpp_warships::game_persistence
