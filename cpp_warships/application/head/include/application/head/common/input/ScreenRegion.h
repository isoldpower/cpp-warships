#pragma once

namespace cpp_warships::head::common::input {
    /** @brief A part of the screen a pointer can be over, or a panel that can take the focus,
     * named in the game's terms. */
    enum class ScreenRegion {
        Elsewhere,
        OwnWaters,
        EnemyWaters,
        Log,
        Fleet,
        Skills,
        Shortcuts,
        Settings,
        SaveList,
        NameEntry,
    };
}  // namespace cpp_warships::head::common::input
