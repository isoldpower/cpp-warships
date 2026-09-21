#include <game_persistence/FilesystemSaveStorage.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <utility>

namespace cpp_warships::game_persistence {
    namespace {
        /** @brief Extension every save file carries. */
        constexpr const char* SAVE_EXTENSION = ".json";
    } // namespace

    FilesystemSaveStorage::FilesystemSaveStorage(std::string directoryPath)
        : directoryPath_(std::move(directoryPath)) {}

    FilesystemSaveStorage FilesystemSaveStorage::inCurrentDirectory() {
        return FilesystemSaveStorage{std::filesystem::current_path().string()};
    }

    std::string FilesystemSaveStorage::pathFor(const std::string& name) const {
        return (std::filesystem::path{directoryPath_} / (name + SAVE_EXTENSION)).string();
    }

    std::vector<std::string> FilesystemSaveStorage::list() const {
        std::vector<std::string> names;

        std::error_code error;
        if (!std::filesystem::is_directory(directoryPath_, error)) {
            return names;
        }

        for (const auto& entry : std::filesystem::directory_iterator{directoryPath_, error}) {
            if (entry.is_regular_file() && entry.path().extension() == SAVE_EXTENSION) {
                names.push_back(entry.path().stem().string());
            }
        }

        return names;
    }

    std::optional<std::string> FilesystemSaveStorage::read(const std::string& name) const {
        std::ifstream file{pathFor(name)};
        std::optional<std::string> contents;

        if (file.is_open()) {
            std::ostringstream buffer;
            buffer << file.rdbuf();
            contents = buffer.str();
        }

        return contents;
    }

    bool FilesystemSaveStorage::write(const std::string& name, const std::string& contents) {
        std::error_code error;
        std::filesystem::create_directories(directoryPath_, error);

        std::ofstream file{pathFor(name)};
        bool isWritten = false;

        if (file.is_open()) {
            file << contents;
            isWritten = file.good();
        }

        return isWritten;
    }

    bool FilesystemSaveStorage::contains(const std::string& name) const {
        std::error_code error;
        return std::filesystem::is_regular_file(pathFor(name), error);
    }

    const std::string& FilesystemSaveStorage::directoryPath() const noexcept {
        return directoryPath_;
    }
} // namespace cpp_warships::game_persistence
