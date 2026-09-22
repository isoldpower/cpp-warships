#pragma once

#include <ftxui/component/component.hpp>

#include <game_tui/EventRouter.h>
#include <game_tui/Intent.h>
#include <game_tui/MatchQuery.h>
#include <game_tui/Screen.h>
#include <game_tui/Theme.h>
#include <game_tui/screens/PlacementState.h>
#include <game_tui/screens/PlacementView.h>

namespace cpp_warships::game_tui {
    /** @brief Wires the placement handlers to the placement view. It holds no input logic. */
    class PlacementController {
    public:
        PlacementController(IntentSink intentSink, const Theme& theme, MatchQuery match);

        [[nodiscard]] ftxui::Component component();

    private:
        const Theme& theme_;
        MatchQuery match_;
        PlacementState state_;
        PlacementView view_;
        EventRouter eventRouter_;
    };

    /** @brief The placement screen, pairing its view with its controller. */
    class PlacementScreen final : public Screen {
    public:
        PlacementScreen(IntentSink intentSink, const Theme& theme, MatchQuery match);

        [[nodiscard]] ScreenKind kind() const override;
        [[nodiscard]] ftxui::Component component() override;

    private:
        PlacementController controller_;
        ftxui::Component component_;
    };
} // namespace cpp_warships::game_tui
