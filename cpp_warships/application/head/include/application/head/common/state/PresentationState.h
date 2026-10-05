#pragma once

#include <application/head/common/state/BattleState.h>
#include <application/head/common/state/MenuState.h>
#include <application/head/common/state/PanelState.h>
#include <application/head/common/state/PlacementState.h>
#include <application/head/common/state/SaveBrowserState.h>
#include <application/head/common/state/SaveNamingState.h>

namespace cpp_warships::head::common::state {
    /** @brief Everything the interface remembers that the game itself does not: where the player
     * is aiming, what they have picked but not yet asked for, how far they have scrolled. */
    struct PresentationState {
        MenuState menu;
        PlacementState placement;
        BattleState battle;
        SaveBrowserState saves;
        SaveNamingState naming;
        PanelState panels;

        /** @brief Whether the player has stepped out to the menu. */
        bool isAtMenu = true;

        /** @brief Whether the player is looking through what has been saved. */
        bool isBrowsingSaves = false;

        /** @brief Whether the player is naming the match they are putting away. */
        bool isNamingSave = false;
    };
}  // namespace cpp_warships::head::common::state
