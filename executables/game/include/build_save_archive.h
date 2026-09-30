#pragma once

#include <application/persistence/SaveArchive.h>
#include <application/persistence/SaveStorage.h>

#include <memory>

namespace cpp_warships::application {
    /** @brief Where a session's saves are kept, and the archive that reads and writes them. */
    struct SaveLibrary {
        std::unique_ptr<persistence::SaveStorage> storage;
        std::unique_ptr<persistence::SaveArchive> archive;
    };

    /** @brief The saves of this build, kept wherever the machine it runs on keeps them.
     * Which storage that is, is settled when the build is configured. */
    [[nodiscard]] SaveLibrary buildSaveLibrary();
}  // namespace cpp_warships::application
