#pragma once

#include <game_tui/Intent.h>
#include <game_tui/ScreenKind.h>

namespace cpp_warships::game_tui {
    /** @brief Asks the application to show another screen. */
    class ShowScreenIntent final : public Intent {
    public:
        explicit ShowScreenIntent(ScreenKind screen);

        void applyTo(Application& application) const override;

    private:
        ScreenKind screen_;
    };
} // namespace cpp_warships::game_tui
