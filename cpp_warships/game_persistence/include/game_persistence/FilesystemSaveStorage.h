#pragma once

#include <string>

#include <game_persistence/SaveStorage.h>

namespace cpp_warships::game_persistence {
    /** @brief Keeps saves as .json files in one directory.
     *  The only part of the project that touches the filesystem. */
    class FilesystemSaveStorage final : public SaveStorage {
    public:
        explicit FilesystemSaveStorage(std::string directoryPath);

        /** @brief A storage rooted at the current working directory. */
        [[nodiscard]] static FilesystemSaveStorage inCurrentDirectory();

        [[nodiscard]] std::vector<std::string> list() const override;
        [[nodiscard]] std::optional<std::string> read(const std::string& name) const override;
        bool write(const std::string& name, const std::string& contents) override;
        [[nodiscard]] bool contains(const std::string& name) const override;

        [[nodiscard]] const std::string& directoryPath() const noexcept;

    private:
        [[nodiscard]] std::string pathFor(const std::string& name) const;

        std::string directoryPath_;
    };
} // namespace cpp_warships::game_persistence
