#pragma once

#include <application/head/common/ScreenKind.h>
#include <application/head/common/input/ScreenRegion.h>

#include <map>

namespace cpp_warships::head::common::state {
    /** @brief How far a panel's content is scrolled, in columns across and lines down. */
    struct ScrollOffset {
        int x = 0;
        int y = 0;

        bool operator==(const ScrollOffset&) const = default;
    };

    /** @brief Which panel holds the focus on each screen, and how far each panel is scrolled. */
    struct PanelState {
        std::map<ScreenKind, input::ScreenRegion> focused;
        std::map<input::ScreenRegion, ScrollOffset> scrolled;
    };
}  // namespace cpp_warships::head::common::state
