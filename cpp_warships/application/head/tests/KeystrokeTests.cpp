#include <application/head/common/input/Keystroke.h>
#include <gtest/gtest.h>

namespace cpp_warships::head::common::input {
    TEST(KeystrokeTests, DefaultsToNothingHavingHappened) {
        const Keystroke stroke;

        EXPECT_EQ(stroke.key, Key::None);
        EXPECT_TRUE(stroke.character.empty());
        EXPECT_EQ(stroke.button, PointerButton::None);
        EXPECT_FALSE(stroke.isPressed);
        EXPECT_EQ(stroke.pointerX, 0);
        EXPECT_EQ(stroke.pointerY, 0);
    }

    TEST(KeystrokeTests, RecognisesATypedCharacter) {
        const Keystroke stroke{.key = Key::Character, .character = "q"};

        EXPECT_TRUE(isCharacter(stroke, "q"));
        EXPECT_FALSE(isCharacter(stroke, "r"));
    }

    TEST(KeystrokeTests, AKeyThatIsNotACharacterTypesNothing) {
        const Keystroke stroke{.key = Key::Enter, .character = "q"};

        EXPECT_FALSE(isCharacter(stroke, "q"));
    }

    TEST(KeystrokeTests, RecognisesAPointer) {
        const Keystroke pointer{.key = Key::Pointer};
        const Keystroke typed{.key = Key::Character, .character = "a"};

        EXPECT_TRUE(isPointer(pointer));
        EXPECT_FALSE(isPointer(typed));
    }

    TEST(KeystrokeTests, RecognisesTheWheelRollingEitherWay) {
        const Keystroke up{.key = Key::Pointer, .button = PointerButton::WheelUp};
        const Keystroke down{.key = Key::Pointer, .button = PointerButton::WheelDown};

        EXPECT_TRUE(isWheelRolled(up));
        EXPECT_TRUE(isWheelRolled(down));
    }

    TEST(KeystrokeTests, AClickIsNotAWheelRoll) {
        const Keystroke click{.key = Key::Pointer, .button = PointerButton::Left};

        EXPECT_FALSE(isWheelRolled(click));
    }

    TEST(KeystrokeTests, AKeyIsNeverAWheelRoll) {
        const Keystroke stroke{.key = Key::ArrowUp};

        EXPECT_FALSE(isWheelRolled(stroke));
    }

    TEST(KeystrokeTests, ATiltedWheelOrAShiftedRollScrollsAcross) {
        const Keystroke tilted{.key = Key::Pointer, .button = PointerButton::WheelRight};
        const Keystroke shifted{
            .key = Key::Pointer,
            .button = PointerButton::WheelDown,
            .isShiftHeld = true
        };
        const Keystroke plain{.key = Key::Pointer, .button = PointerButton::WheelDown};

        EXPECT_TRUE(isWheelRolled(tilted));
        EXPECT_TRUE(isWheelRolledAcross(tilted));
        EXPECT_TRUE(isWheelRolledAcross(shifted));
        EXPECT_FALSE(isWheelRolledAcross(plain));
    }
}  // namespace cpp_warships::head::common::input
