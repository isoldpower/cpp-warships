#include <application/persistence/BrowserSaveStorage.h>
#include <build_save_archive.h>

#include <string>
#include <utility>

namespace cpp_warships::application {
    namespace {
        /** @brief What every save's key starts with, so the game shares local storage
         * with whatever else the page keeps there. */
        const std::string SAVE_KEY_PREFIX = "cpp-warships/save/";
    }  // namespace

    SaveLibrary buildSaveLibrary() {
        auto storage = std::make_unique<persistence::BrowserSaveStorage>(SAVE_KEY_PREFIX);
        auto archive = std::make_unique<persistence::SaveArchive>(*storage);

        return SaveLibrary{.storage = std::move(storage), .archive = std::move(archive)};
    }
}  // namespace cpp_warships::application
