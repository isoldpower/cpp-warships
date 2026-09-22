#pragma once

#include <ftxui/component/component.hpp>

#include <game_tui/ScreenKind.h>

namespace cpp_warships::game_tui {
    /** @brief One screen of the interface, as a component the application can show.
     *  A screen owns a view that draws and a controller that turns input into intents. */
    class Screen {
    public:
        virtual ~Screen() = default;

        [[nodiscard]] virtual ScreenKind kind() const = 0;

        /** @brief The component drawn and given events while this screen is showing. */
        [[nodiscard]] virtual ftxui::Component component() = 0;
    };
} // namespace cpp_warships::game_tui
