#include <application/head/common/input/KeyCodes.h>
#include <application/head/common/input/Keystroke.h>
#include <gtest/gtest.h>

#include <optional>

namespace cpp_warships::head::common::input {
    namespace {
        Keystroke read(const char* sequence) {
            const std::optional<Keystroke> stroke = keystrokeOfKeyCode(sequence);
            EXPECT_TRUE(stroke.has_value()) << "not read as a key code: " << sequence;
            return stroke.value_or(Keystroke{});
        }
    }  // namespace

    TEST(KeyCodeTests, ReadsALetterTypedInTheUsLayout) {
        const Keystroke stroke = read("\x1b[107u");

        EXPECT_EQ(stroke.key, Key::Character);
        EXPECT_EQ(stroke.character, "k");
    }

    TEST(KeyCodeTests, ReadsAKeyInAnotherLayoutAsTheUsLetterOnIt) {
        EXPECT_EQ(read("\x1b[1083::107u").character, "k");
        EXPECT_EQ(read("\x1b[966::102u").character, "f");
    }

    TEST(KeyCodeTests, ShiftsTheUsLetterWhenShiftIsHeld) {
        EXPECT_EQ(read("\x1b[1083:1051:107;2u").character, "K");
        EXPECT_EQ(read("\x1b[49:33;2u").character, "!");
    }

    TEST(KeyCodeTests, ReadsTheKeysThatTypeNothingByName) {
        EXPECT_EQ(read("\x1b[13u").key, Key::Enter);
        EXPECT_EQ(read("\x1b[27u").key, Key::Escape);
        EXPECT_EQ(read("\x1b[9u").key, Key::Tab);
        EXPECT_EQ(read("\x1b[127u").key, Key::Backspace);
        EXPECT_EQ(read("\x1b[32u").character, " ");
    }

    TEST(KeyCodeTests, ReadsControlCAsAnInterruptInAnyLayout) {
        EXPECT_EQ(read("\x1b[99;5u").key, Key::Interrupt);
        EXPECT_EQ(read("\x1b[1089::99;5u").key, Key::Interrupt);
    }

    TEST(KeyCodeTests, IgnoresModifiedKeysLoneModifiersAndReleases) {
        EXPECT_EQ(read("\x1b[107;5u").key, Key::None);
        EXPECT_EQ(read("\x1b[57441;2u").key, Key::None);
        EXPECT_EQ(read("\x1b[107;1:3u").key, Key::None);
    }

    TEST(KeyCodeTests, LeavesAnythingThatIsNotAKeyCodeAlone) {
        EXPECT_EQ(keystrokeOfKeyCode("k"), std::nullopt);
        EXPECT_EQ(keystrokeOfKeyCode("\x1b[A"), std::nullopt);
        EXPECT_EQ(keystrokeOfKeyCode("\x1b[1;2A"), std::nullopt);
        EXPECT_EQ(keystrokeOfKeyCode("\x1b[?15u"), std::nullopt);
    }

    TEST(KeyCodeTests, RecognisesOnlyTheTerminalsAnswerToTheQuery) {
        EXPECT_TRUE(isKeyCodesAnswer("\x1b[?12u"));
        EXPECT_TRUE(isKeyCodesAnswer("\x1b[?0u"));
        EXPECT_FALSE(isKeyCodesAnswer("\x1b[?u"));
        EXPECT_FALSE(isKeyCodesAnswer("\x1b[107u"));
        EXPECT_FALSE(isKeyCodesAnswer("\x1b[?62;22c"));
    }
}  // namespace cpp_warships::head::common::input
