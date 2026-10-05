#pragma once

#include <string>

namespace cpp_warships::head::common::input {
    /** @brief A key the player pressed, named for what it means rather than
     * what sent it. */
    enum class Key {
        None,
        Character,
        Enter,
        Escape,
        Tab,
        Backspace,
        Delete,
        ArrowUp,
        ArrowDown,
        ArrowLeft,
        ArrowRight,
        PageUp,
        PageDown,
        ShiftArrowUp,
        ShiftArrowDown,
        ShiftArrowLeft,
        ShiftArrowRight,

        Interrupt,

        Pointer,
    };

    /** @brief Which part of a pointing device was used, when one was used at all. */
    enum class PointerButton {
        None,
        Left,
        Right,
        Middle,
        WheelUp,
        WheelDown,
        WheelLeft,
        WheelRight,
    };

    /** @brief One thing the player did, in terms no particular terminal library owns. */
    struct Keystroke {
        Key key = Key::None;

        /** @brief What was typed, when a plain character was. */
        std::string character{};

        PointerButton button = PointerButton::None;
        bool isPressed = false;
        int pointerX = 0;
        int pointerY = 0;

        /** @brief Whether shift was held while the pointing device was used. */
        bool isShiftHeld = false;
    };

    /** @brief Whether @p stroke is the given key being typed. */
    [[nodiscard]] bool isCharacter(const Keystroke& stroke, const std::string& character);

    /** @brief Whether @p stroke came from a pointing device at all. */
    [[nodiscard]] bool isPointer(const Keystroke& stroke);

    /** @brief Whether @p stroke is the wheel being rolled any way at all. */
    [[nodiscard]] bool isWheelRolled(const Keystroke& stroke);

    /** @brief Whether @p stroke rolls the wheel sideways: tilted, or rolled with shift held,
     * the way terminals and browsers scroll across. */
    [[nodiscard]] bool isWheelRolledAcross(const Keystroke& stroke);
}  // namespace cpp_warships::head::common::input
