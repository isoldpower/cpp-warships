#include <application/head/common/input/Keystroke.h>

namespace cpp_warships::head::common::input {
    bool isCharacter(const Keystroke& stroke, const std::string& character) {
        return stroke.key == Key::Character && stroke.character == character;
    }

    bool isPointer(const Keystroke& stroke) {
        return stroke.key == Key::Pointer;
    }

    bool isWheelRolled(const Keystroke& stroke) {
        return isPointer(stroke) && (stroke.button == PointerButton::WheelUp ||
                                     stroke.button == PointerButton::WheelDown ||
                                     stroke.button == PointerButton::WheelLeft ||
                                     stroke.button == PointerButton::WheelRight);
    }

    bool isWheelRolledAcross(const Keystroke& stroke) {
        if (!isWheelRolled(stroke)) {
            return false;
        }

        return stroke.isShiftHeld || stroke.button == PointerButton::WheelLeft ||
               stroke.button == PointerButton::WheelRight;
    }
}  // namespace cpp_warships::head::common::input
