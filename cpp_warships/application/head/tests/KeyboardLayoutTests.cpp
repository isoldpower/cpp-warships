#include <application/head/common/PresentationContext.h>
#include <application/head/common/input/BattleInput.h>
#include <application/head/common/input/KeyCodes.h>
#include <application/head/common/input/Keystroke.h>
#include <application/head/common/input/MenuInput.h>
#include <application/head/common/input/PlacementInput.h>
#include <application/model/ApplicationContext.h>
#include <application/model/WarshipsGame.h>
#include <application/model/events/GameEvent.h>
#include <application/persistence/MemorySaveStorage.h>
#include <application/persistence/SaveArchive.h>
#include <gtest/gtest.h>

#include <optional>
#include <string>

namespace cpp_warships::head::common::input {
    namespace {
        constexpr int BOARD_SIZE = 10;

        /** @brief What a kitty-protocol terminal reports for a key, named by the layout it was
         * typed in and the US-layout key it sits on. */
        constexpr const char* ENGLISH_F = "\x1b[102u";
        constexpr const char* ENGLISH_K = "\x1b[107u";
        constexpr const char* RUSSIAN_B = "\x1b[1080::98u";
        constexpr const char* RUSSIAN_K = "\x1b[1083::107u";
        constexpr const char* RUSSIAN_Q = "\x1b[1081::113u";
        constexpr const char* RUSSIAN_R = "\x1b[1082::114u";
        constexpr const char* GREEK_K = "\x1b[954::107u";
        constexpr const char* ESCAPE = "\x1b[27u";

        /** @brief A match being placed, presented, all kept alive together. */
        class MatchFixture {
        public:
            MatchFixture()
                : archive_(storage_)
                , game_(randomEngine_, archive_)
                , application_(game_)
                , context_(application_) {
                game_.play().startNewMatch(BOARD_SIZE);
            }

            PresentationContext& context() noexcept {
                return context_;
            }

            model::WarshipsGame& game() noexcept {
                return game_;
            }

        private:
            flow::RandomEngine randomEngine_{17U};
            persistence::MemorySaveStorage storage_;
            persistence::SaveArchive archive_;
            model::WarshipsGame game_;
            model::ApplicationContext application_;
            PresentationContext context_;
        };

        /** @brief The keystroke a terminal speaking the kitty keyboard protocol reports. */
        Keystroke reported(const std::string& sequence) {
            const std::optional<Keystroke> stroke = keystrokeOfKeyCode(sequence);
            EXPECT_TRUE(stroke.has_value()) << "not a key code";
            return stroke.value_or(Keystroke{});
        }

        /** @brief The keystroke a terminal without the protocol sends: just the character. */
        Keystroke typed(const std::string& character) {
            return Keystroke{.key = Key::Character, .character = character};
        }

        template <typename Wanted>
        [[nodiscard]] bool asks(const std::optional<model::events::GameEvent>& event) {
            return event.has_value() && model::events::isKind<Wanted>(*event);
        }
    }  // namespace

    TEST(KeyboardLayoutTests, ShortcutsKeepWorkingAsTheLayoutChangesMidGame) {
        MatchFixture fixture;
        PlacementInput placement{fixture.context()};
        BattleInput battle{fixture.context()};
        MenuInput menu{fixture.context()};

        EXPECT_TRUE(
            asks<model::events::FleetShuffleRequested>(placement.interpret(reported(ENGLISH_F)))
        );
        fixture.game().play().shuffleFleet();
        EXPECT_TRUE(
            asks<model::events::BattleBeginRequested>(placement.interpret(reported(RUSSIAN_B)))
        );
        fixture.game().play().beginBattle();

        EXPECT_TRUE(asks<model::events::SkillUseRequested>(battle.interpret(reported(RUSSIAN_K))));
        EXPECT_TRUE(asks<model::events::SkillUseRequested>(battle.interpret(reported(GREEK_K))));
        EXPECT_TRUE(asks<model::events::SkillUseRequested>(battle.interpret(reported(ENGLISH_K))));

        EXPECT_TRUE(asks<model::events::MenuReturnRequested>(battle.interpret(reported(ESCAPE))));
        EXPECT_TRUE(asks<model::events::SessionQuitRequested>(menu.interpret(reported(RUSSIAN_Q))));
    }

    TEST(KeyboardLayoutTests, ATerminalWithoutKeyCodesHearsShortcutsOnlyInALatinLayout) {
        MatchFixture fixture;
        PlacementInput placement{fixture.context()};

        EXPECT_FALSE(placement.interpret(typed("а")).has_value());
        EXPECT_TRUE(asks<model::events::FleetShuffleRequested>(placement.interpret(typed("f"))));
    }

    TEST(KeyboardLayoutTests, ASwitchedLayoutNeverFiresAShortcutOnTheWrongKey) {
        MatchFixture fixture;
        fixture.game().play().shuffleFleet();
        fixture.game().play().beginBattle();
        BattleInput battle{fixture.context()};

        EXPECT_FALSE(battle.interpret(reported(RUSSIAN_R)).has_value());
    }
}  // namespace cpp_warships::head::common::input
