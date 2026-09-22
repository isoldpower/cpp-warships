#pragma once

#include <string>

#include <game_tui/Intent.h>

namespace cpp_warships::game_tui {
    /** @brief Asks the application to dress itself in another theme. */
    class ChangeThemeIntent final : public Intent {
    public:
        explicit ChangeThemeIntent(std::string themeName);

        void applyTo(Application& application) const override;

    private:
        std::string themeName_;
    };
} // namespace cpp_warships::game_tui
