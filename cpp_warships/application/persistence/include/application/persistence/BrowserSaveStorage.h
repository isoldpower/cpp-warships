#pragma once

#include <application/persistence/SaveStorage.h>

#include <string>

namespace cpp_warships::persistence {
    /** @brief Keeps saves in the browser's local storage, so they outlive the page.
     * Every save is one entry, named for the key prefix this was built with. */
    class BrowserSaveStorage final : public SaveStorage {
    public:
        explicit BrowserSaveStorage(std::string keyPrefix);

        [[nodiscard]] std::vector<std::string> list() const override;
        [[nodiscard]] std::optional<std::string> read(const std::string& name) const override;
        bool write(const std::string& name, const std::string& contents) override;
        [[nodiscard]] bool contains(const std::string& name) const override;
        bool remove(const std::string& name) override;

        [[nodiscard]] const std::string& keyPrefix() const noexcept;

    private:
        [[nodiscard]] std::string keyFor(const std::string& name) const;

        std::string keyPrefix_;
    };
}  // namespace cpp_warships::persistence
