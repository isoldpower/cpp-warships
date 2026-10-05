#include <application/core/Coordinate.h>
#include <application/core/Direction.h>
#include <application/head/common/PresentationContext.h>
#include <application/head/common/ScreenKind.h>
#include <application/head/common/input/BattleInput.h>
#include <application/head/common/input/EventBus.h>
#include <application/head/common/input/Keystroke.h>
#include <application/head/common/input/MenuInput.h>
#include <application/head/common/input/PanelScrolling.h>
#include <application/head/common/input/PlacementInput.h>
#include <application/head/common/input/SaveBrowserInput.h>
#include <application/head/common/input/SaveNamingInput.h>
#include <application/head/common/input/ScreenInput.h>
#include <application/model/ApplicationContext.h>
#include <application/model/WarshipsGame.h>
#include <application/model/events/GameEvent.h>
#include <application/persistence/MemorySaveStorage.h>
#include <application/persistence/SaveArchive.h>
#include <gtest/gtest.h>

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>

namespace cpp_warships::head::common::input {
    namespace {
        constexpr int BOARD_SIZE = 10;

        /** @brief A presentation context over a real game, all kept alive together. */
        class PresentationFixture {
        public:
            PresentationFixture()
                : archive_(storage_)
                , game_(randomEngine_, archive_)
                , application_(game_)
                , context_(application_) {}

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

        Keystroke typed(const std::string& character) {
            return Keystroke{.key = Key::Character, .character = character};
        }

        Keystroke pressed(Key key) {
            return Keystroke{.key = key};
        }
    }  // namespace

    TEST(MenuInputTests, EnterAsksForAMatchOnTheChosenBoardSize) {
        PresentationFixture fixture;
        MenuInput input{fixture.context()};
        fixture.context().state().menu.selectedBoardSize = 14;

        const std::optional<model::events::GameEvent> event = input.interpret(pressed(Key::Enter));

        ASSERT_TRUE(event.has_value());
        ASSERT_TRUE(model::events::isKind<model::events::MatchStartRequested>(*event));
        EXPECT_EQ(std::get<model::events::MatchStartRequested>(*event).boardSize, 14);
    }

    TEST(MenuInputTests, ArrowsResizeTheBoardWithoutAskingTheGameForAnything) {
        PresentationFixture fixture;
        MenuInput input{fixture.context()};
        fixture.context().state().menu.selectedBoardSize = 10;

        EXPECT_FALSE(input.interpret(pressed(Key::ArrowRight)).has_value());
        EXPECT_EQ(fixture.context().state().menu.selectedBoardSize, 12);

        EXPECT_FALSE(input.interpret(pressed(Key::ArrowLeft)).has_value());
        EXPECT_EQ(fixture.context().state().menu.selectedBoardSize, 10);
    }

    TEST(MenuInputTests, TheBoardSizeStopsAtItsLimits) {
        PresentationFixture fixture;
        MenuInput input{fixture.context()};

        for (int press = 0; press < 20; ++press) {
            (void)input.interpret(pressed(Key::ArrowLeft));
        }
        EXPECT_EQ(fixture.context().state().menu.selectedBoardSize, 8);

        for (int press = 0; press < 20; ++press) {
            (void)input.interpret(pressed(Key::ArrowRight));
        }
        EXPECT_EQ(fixture.context().state().menu.selectedBoardSize, 20);
    }

    TEST(MenuInputTests, TypingTCyclesTheTheme) {
        PresentationFixture fixture;
        MenuInput input{fixture.context()};
        const std::string before = fixture.context().theme().name;

        EXPECT_FALSE(input.interpret(typed("t")).has_value());

        EXPECT_NE(fixture.context().theme().name, before);
    }

    TEST(MenuInputTests, TypingRAsksToResume) {
        PresentationFixture fixture;
        MenuInput input{fixture.context()};

        const std::optional<model::events::GameEvent> event = input.interpret(typed("r"));

        ASSERT_TRUE(event.has_value());
        EXPECT_TRUE(model::events::isKind<model::events::MatchResumeRequested>(*event));
    }

    TEST(MenuInputTests, TypingLOpensTheSaves) {
        PresentationFixture fixture;
        MenuInput input{fixture.context()};

        const std::optional<model::events::GameEvent> event = input.interpret(typed("l"));

        ASSERT_TRUE(event.has_value());
        EXPECT_TRUE(model::events::isKind<model::events::SaveBrowserRequested>(*event));
    }

    TEST(MenuInputTests, TypingSAsksWhatToCallTheMatch) {
        PresentationFixture fixture;
        MenuInput input{fixture.context()};

        const std::optional<model::events::GameEvent> event = input.interpret(typed("s"));

        ASSERT_TRUE(event.has_value());
        EXPECT_TRUE(model::events::isKind<model::events::SaveNamingRequested>(*event));
    }

    TEST(MenuInputTests, TypingQAsksToStop) {
        PresentationFixture fixture;
        MenuInput input{fixture.context()};

        const std::optional<model::events::GameEvent> event = input.interpret(typed("q"));

        ASSERT_TRUE(event.has_value());
        EXPECT_TRUE(model::events::isKind<model::events::SessionQuitRequested>(*event));
    }

    TEST(MenuInputTests, AKeyBoundToNothingAsksForNothing) {
        PresentationFixture fixture;
        MenuInput input{fixture.context()};

        EXPECT_FALSE(input.interpret(typed("z")).has_value());
        EXPECT_FALSE(input.interpret(pressed(Key::Tab)).has_value());
    }

    TEST(PlacementInputTests, ArrowsMoveTheCursorWithoutAskingForAnything) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        PlacementInput input{fixture.context()};
        fixture.context().state().placement.cursor = core::Coordinate{0, 0};

        EXPECT_FALSE(input.interpret(pressed(Key::ArrowRight)).has_value());

        EXPECT_EQ(fixture.context().state().placement.cursor, (core::Coordinate{1, 0}));
    }

    TEST(PlacementInputTests, TheCursorStaysOnTheBoard) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        PlacementInput input{fixture.context()};
        fixture.context().state().placement.cursor = core::Coordinate{0, 0};

        (void)input.interpret(pressed(Key::ArrowLeft));
        (void)input.interpret(pressed(Key::ArrowUp));

        EXPECT_EQ(fixture.context().state().placement.cursor, (core::Coordinate{0, 0}));
    }

    TEST(PlacementInputTests, TypingRTurnsTheShipInHand) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        PlacementInput input{fixture.context()};
        fixture.context().state().placement.direction = core::Direction::Horizontal;

        EXPECT_FALSE(input.interpret(typed("r")).has_value());
        EXPECT_EQ(fixture.context().state().placement.direction, core::Direction::Vertical);

        EXPECT_FALSE(input.interpret(typed("r")).has_value());
        EXPECT_EQ(fixture.context().state().placement.direction, core::Direction::Horizontal);
    }

    TEST(PlacementInputTests, TabPicksAnotherShipLength) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        PlacementInput input{fixture.context()};
        const int before = fixture.context().state().placement.preferredShipLength;

        EXPECT_FALSE(input.interpret(pressed(Key::Tab)).has_value());

        EXPECT_NE(fixture.context().state().placement.preferredShipLength, before);
    }

    TEST(PlacementInputTests, TypingBOpensFire) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        PlacementInput input{fixture.context()};

        const std::optional<model::events::GameEvent> event = input.interpret(typed("b"));

        ASSERT_TRUE(event.has_value());
        EXPECT_TRUE(model::events::isKind<model::events::BattleBeginRequested>(*event));
    }

    TEST(PlacementInputTests, EnterAsksForTheShipToBeLaid) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        PlacementInput input{fixture.context()};

        const std::optional<model::events::GameEvent> event = input.interpret(pressed(Key::Enter));

        ASSERT_TRUE(event.has_value());
        EXPECT_TRUE(model::events::isKind<model::events::ShipPlacementRequested>(*event));
    }

    TEST(PlacementInputTests, BackspaceAsksForAShipToBeTakenBack) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        PlacementInput input{fixture.context()};

        const std::optional<model::events::GameEvent> event =
            input.interpret(pressed(Key::Backspace));

        ASSERT_TRUE(event.has_value());
        EXPECT_TRUE(model::events::isKind<model::events::ShipRemovalRequested>(*event));
    }

    TEST(PlacementInputTests, TypingFAsksForTheFleetToBeLaidOut) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        PlacementInput input{fixture.context()};

        const std::optional<model::events::GameEvent> event = input.interpret(typed("f"));

        ASSERT_TRUE(event.has_value());
        EXPECT_TRUE(model::events::isKind<model::events::FleetShuffleRequested>(*event));
    }

    TEST(PlacementInputTests, EscapeStepsBackToTheMenu) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        PlacementInput input{fixture.context()};

        const std::optional<model::events::GameEvent> event = input.interpret(pressed(Key::Escape));

        ASSERT_TRUE(event.has_value());
        EXPECT_TRUE(model::events::isKind<model::events::MenuReturnRequested>(*event));
    }

    TEST(BattleInputTests, EnterFiresAtWhereThePlayerIsAiming) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        fixture.game().play().shuffleFleet();
        fixture.game().play().beginBattle();
        BattleInput input{fixture.context()};
        fixture.context().state().battle.target = core::Coordinate{3, 4};

        const std::optional<model::events::GameEvent> event = input.interpret(pressed(Key::Enter));

        ASSERT_TRUE(event.has_value());
        ASSERT_TRUE(model::events::isKind<model::events::ShotRequested>(*event));
        EXPECT_EQ(std::get<model::events::ShotRequested>(*event).target, (core::Coordinate{3, 4}));
    }

    TEST(BattleInputTests, ArrowsMoveTheAimWithoutFiring) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        fixture.game().play().shuffleFleet();
        fixture.game().play().beginBattle();
        BattleInput input{fixture.context()};
        fixture.context().state().battle.target = core::Coordinate{0, 0};

        EXPECT_FALSE(input.interpret(pressed(Key::ArrowDown)).has_value());

        EXPECT_EQ(fixture.context().state().battle.target, (core::Coordinate{0, 1}));
    }

    TEST(BattleInputTests, TypingKSpendsASkill) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        fixture.game().play().shuffleFleet();
        fixture.game().play().beginBattle();
        BattleInput input{fixture.context()};

        const std::optional<model::events::GameEvent> event = input.interpret(typed("k"));

        ASSERT_TRUE(event.has_value());
        EXPECT_TRUE(model::events::isKind<model::events::SkillUseRequested>(*event));
    }

    TEST(BattleInputTests, EscapeStepsBackToTheMenu) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        fixture.game().play().shuffleFleet();
        fixture.game().play().beginBattle();
        BattleInput input{fixture.context()};

        const std::optional<model::events::GameEvent> event = input.interpret(pressed(Key::Escape));

        ASSERT_TRUE(event.has_value());
        EXPECT_TRUE(model::events::isKind<model::events::MenuReturnRequested>(*event));
    }

    TEST(SaveBrowserInputTests, EnterWithNothingSavedAsksForNothing) {
        PresentationFixture fixture;
        SaveBrowserInput input{fixture.context()};

        EXPECT_FALSE(input.interpret(pressed(Key::Enter)).has_value());
    }

    TEST(SaveBrowserInputTests, EnterLoadsTheSaveBeingLookedAt) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        (void)fixture.game().saves().saveMatch("my game");
        SaveBrowserInput input{fixture.context()};

        const std::optional<model::events::GameEvent> event = input.interpret(pressed(Key::Enter));

        ASSERT_TRUE(event.has_value());
        ASSERT_TRUE(model::events::isKind<model::events::MatchLoadRequested>(*event));
        EXPECT_EQ(
            std::get<model::events::MatchLoadRequested>(*event).name,
            fixture.game().saves().savedMatches().front().id
        );
    }

    TEST(SaveBrowserInputTests, TypingDThrowsTheSaveBeingLookedAtAway) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        (void)fixture.game().saves().saveMatch("my game");
        SaveBrowserInput input{fixture.context()};

        const std::optional<model::events::GameEvent> event = input.interpret(typed("d"));

        ASSERT_TRUE(event.has_value());
        EXPECT_TRUE(model::events::isKind<model::events::SaveDeleteRequested>(*event));
    }

    TEST(SaveBrowserInputTests, ArrowsMoveThroughTheListWithoutLoadingAnything) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        (void)fixture.game().saves().saveMatch("my game");
        SaveBrowserInput input{fixture.context()};

        EXPECT_FALSE(input.interpret(pressed(Key::ArrowDown)).has_value());

        EXPECT_EQ(fixture.context().state().saves.selectedIndex, 0);
    }

    TEST(SaveBrowserInputTests, EscapeStepsBackToTheMenu) {
        PresentationFixture fixture;
        SaveBrowserInput input{fixture.context()};

        const std::optional<model::events::GameEvent> event = input.interpret(pressed(Key::Escape));

        ASSERT_TRUE(event.has_value());
        EXPECT_TRUE(model::events::isKind<model::events::MenuReturnRequested>(*event));
    }

    TEST(SaveNamingInputTests, TypingBuildsUpTheName) {
        PresentationFixture fixture;
        SaveNamingInput input{fixture.context()};

        EXPECT_FALSE(input.interpret(typed("a")).has_value());
        EXPECT_FALSE(input.interpret(typed("b")).has_value());

        EXPECT_EQ(fixture.context().state().naming.typedName, "ab");
    }

    TEST(SaveNamingInputTests, BackspaceRubsTheLastLetterOut) {
        PresentationFixture fixture;
        SaveNamingInput input{fixture.context()};
        fixture.context().state().naming.typedName = "abc";

        (void)input.interpret(pressed(Key::Backspace));

        EXPECT_EQ(fixture.context().state().naming.typedName, "ab");
    }

    TEST(SaveNamingInputTests, BackspaceOnAnEmptyNameIsHarmless) {
        PresentationFixture fixture;
        SaveNamingInput input{fixture.context()};

        EXPECT_NO_THROW((void)input.interpret(pressed(Key::Backspace)));

        EXPECT_TRUE(fixture.context().state().naming.typedName.empty());
    }

    TEST(SaveNamingInputTests, AnEmptyNameCannotBeConfirmed) {
        PresentationFixture fixture;
        SaveNamingInput input{fixture.context()};

        EXPECT_FALSE(input.interpret(pressed(Key::Enter)).has_value());
    }

    TEST(SaveNamingInputTests, EnterPutsTheMatchAwayUnderTheNameTyped) {
        PresentationFixture fixture;
        SaveNamingInput input{fixture.context()};
        fixture.context().state().naming.typedName = "my game";

        const std::optional<model::events::GameEvent> event = input.interpret(pressed(Key::Enter));

        ASSERT_TRUE(event.has_value());
        ASSERT_TRUE(model::events::isKind<model::events::MatchSaveAndQuitRequested>(*event));
        EXPECT_EQ(std::get<model::events::MatchSaveAndQuitRequested>(*event).name, "my game");
    }

    TEST(SaveNamingInputTests, ANameStopsGrowingAtItsLimit) {
        PresentationFixture fixture;
        SaveNamingInput input{fixture.context()};

        for (int press = 0; press < 60; ++press) {
            (void)input.interpret(typed("x"));
        }

        EXPECT_EQ(fixture.context().state().naming.typedName.size(), 40U);
    }

    TEST(SaveNamingInputTests, EscapeStepsBackToTheMenu) {
        PresentationFixture fixture;
        SaveNamingInput input{fixture.context()};

        const std::optional<model::events::GameEvent> event = input.interpret(pressed(Key::Escape));

        ASSERT_TRUE(event.has_value());
        EXPECT_TRUE(model::events::isKind<model::events::MenuReturnRequested>(*event));
    }

    TEST(EventBusTests, AScreenNobodyReadsAsksForNothing) {
        EventBus bus;

        EXPECT_FALSE(bus.interpret(ScreenKind::Menu, pressed(Key::Enter)).has_value());
    }

    TEST(EventBusTests, ReadsAStrokeThroughTheScreenItBelongsTo) {
        PresentationFixture fixture;
        EventBus bus;
        bus.readScreenWith(ScreenKind::Menu, std::make_unique<MenuInput>(fixture.context()));

        const std::optional<model::events::GameEvent> event =
            bus.interpret(ScreenKind::Menu, typed("q"));

        ASSERT_TRUE(event.has_value());
        EXPECT_TRUE(model::events::isKind<model::events::SessionQuitRequested>(*event));
    }

    TEST(EventBusTests, AStrokeMeansNothingOnAScreenThatIsNotShowing) {
        PresentationFixture fixture;
        EventBus bus;
        bus.readScreenWith(ScreenKind::Menu, std::make_unique<MenuInput>(fixture.context()));

        EXPECT_FALSE(bus.interpret(ScreenKind::Battle, typed("q")).has_value());
    }

    TEST(EventBusTests, PuttingAScreenInChargeAgainReplacesItsReader) {
        PresentationFixture fixture;
        EventBus bus;
        bus.readScreenWith(ScreenKind::Menu, std::make_unique<MenuInput>(fixture.context()));
        bus.readScreenWith(ScreenKind::Menu, std::make_unique<SaveNamingInput>(fixture.context()));

        EXPECT_FALSE(bus.interpret(ScreenKind::Menu, typed("q")).has_value());
        EXPECT_EQ(fixture.context().state().naming.typedName, "q");
    }

    TEST(PanelInputTests, ShiftedArrowsHandTheFocusOnWithoutMovingTheAim) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        fixture.game().play().shuffleFleet();
        fixture.game().play().beginBattle();
        BattleInput input{fixture.context()};
        fixture.context().geometry().rememberPanel(
            ScreenRegion::EnemyWaters,
            PanelExtent{.left = 0, .top = 0, .width = 40, .height = 20}
        );
        fixture.context().geometry().rememberPanel(
            ScreenRegion::Log,
            PanelExtent{.left = 42, .top = 0, .width = 20, .height = 10}
        );

        EXPECT_FALSE(input.interpret(pressed(Key::ShiftArrowLeft)).has_value());

        EXPECT_EQ(
            focusedPanel(fixture.context().state(), ScreenKind::Battle),
            ScreenRegion::EnemyWaters
        );
        EXPECT_EQ(fixture.context().state().battle.target, (core::Coordinate{0, 0}));
    }

    TEST(PanelInputTests, PageDownPagesThroughTheFocusedPanel) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        PlacementInput input{fixture.context()};
        fixture.context().state().panels.focused[ScreenKind::Placement] = ScreenRegion::Fleet;
        fixture.context().geometry().rememberPanel(
            ScreenRegion::Fleet,
            PanelExtent{
                .left = 0,
                .top = 0,
                .width = 20,
                .height = 4,
                .contentWidth = 20,
                .contentHeight = 10
            }
        );

        EXPECT_FALSE(input.interpret(pressed(Key::PageDown)).has_value());

        EXPECT_EQ(scrollOf(fixture.context().state(), ScreenRegion::Fleet).y, 3);
    }

    TEST(PanelInputTests, PageUpReadsFurtherBackThroughTheLog) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        fixture.game().play().shuffleFleet();
        fixture.game().play().beginBattle();
        BattleInput input{fixture.context()};
        fixture.context().geometry().rememberPanel(
            ScreenRegion::Log,
            PanelExtent{
                .left = 0,
                .top = 0,
                .width = 20,
                .height = 4,
                .contentWidth = 20,
                .contentHeight = 10
            }
        );

        EXPECT_FALSE(input.interpret(pressed(Key::PageUp)).has_value());

        EXPECT_EQ(fixture.context().state().battle.logScroll, 3);
    }

    TEST(PanelInputTests, TheWheelScrollsWhicheverPanelItIsRolledOver) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        fixture.game().play().shuffleFleet();
        fixture.game().play().beginBattle();
        BattleInput input{fixture.context()};
        fixture.context().geometry().rememberPanel(
            ScreenRegion::Skills,
            PanelExtent{
                .left = 50,
                .top = 0,
                .width = 20,
                .height = 3,
                .contentWidth = 20,
                .contentHeight = 10
            }
        );
        const Keystroke rolledDown{
            .key = Key::Pointer,
            .button = PointerButton::WheelDown,
            .pointerX = 55,
            .pointerY = 1
        };

        EXPECT_FALSE(input.interpret(rolledDown).has_value());

        EXPECT_EQ(scrollOf(fixture.context().state(), ScreenRegion::Skills).y, PANEL_SCROLL_STEP);
        EXPECT_EQ(focusedPanel(fixture.context().state(), ScreenKind::Battle), ScreenRegion::Log);
    }

    TEST(PanelInputTests, WalkingTheAimOffTheWindowScrollsItBackIntoView) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        fixture.game().play().shuffleFleet();
        fixture.game().play().beginBattle();
        BattleInput input{fixture.context()};
        fixture.context()
            .geometry()
            .rememberBoard(ScreenRegion::EnemyWaters, 3, 1, 39, 19, 10, 10, 4, 2);
        fixture.context().geometry().rememberPanel(
            ScreenRegion::EnemyWaters,
            PanelExtent{
                .left = 0,
                .top = 0,
                .width = 42,
                .height = 6,
                .contentWidth = 42,
                .contentHeight = 20
            }
        );
        fixture.context().state().battle.target = core::Coordinate{0, 2};

        EXPECT_FALSE(input.interpret(pressed(Key::ArrowDown)).has_value());

        EXPECT_EQ(fixture.context().state().battle.target, (core::Coordinate{0, 3}));
        EXPECT_EQ(scrollOf(fixture.context().state(), ScreenRegion::EnemyWaters).y, 3);
    }

    TEST(PanelInputTests, ChoosingASaveScrollsItIntoView) {
        PresentationFixture fixture;
        for (const std::string name : {"first", "second", "third"}) {
            fixture.game().play().startNewMatch(BOARD_SIZE);
            (void)fixture.game().saves().saveMatch(name);
        }
        SaveBrowserInput input{fixture.context()};
        fixture.context().geometry().rememberPanel(
            ScreenRegion::SaveList,
            PanelExtent{
                .left = 0,
                .top = 0,
                .width = 40,
                .height = 1,
                .contentWidth = 40,
                .contentHeight = 3
            }
        );

        EXPECT_FALSE(input.interpret(pressed(Key::ArrowDown)).has_value());

        EXPECT_EQ(fixture.context().state().saves.selectedIndex, 1);
        EXPECT_EQ(scrollOf(fixture.context().state(), ScreenRegion::SaveList).y, 1);
    }

    TEST(PanelInputTests, TheWheelOverTheBoardScrollsItRatherThanPickingAShip) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        PlacementInput input{fixture.context()};
        fixture.context()
            .geometry()
            .rememberBoard(ScreenRegion::OwnWaters, 3, 1, 39, 19, 10, 10, 4, 2);
        fixture.context().geometry().rememberPanel(
            ScreenRegion::OwnWaters,
            PanelExtent{
                .left = 0,
                .top = 0,
                .width = 20,
                .height = 6,
                .contentWidth = 42,
                .contentHeight = 20
            }
        );
        const int lengthBefore = fixture.context().state().placement.preferredShipLength;
        const auto rolledOverACell = [](bool isShiftHeld) {
            return Keystroke{
                .key = Key::Pointer,
                .button = PointerButton::WheelDown,
                .pointerX = 4,
                .pointerY = 1,
                .isShiftHeld = isShiftHeld
            };
        };

        EXPECT_FALSE(input.interpret(rolledOverACell(false)).has_value());
        EXPECT_FALSE(input.interpret(rolledOverACell(true)).has_value());

        EXPECT_EQ(
            scrollOf(fixture.context().state(), ScreenRegion::OwnWaters).y,
            PANEL_SCROLL_STEP
        );
        EXPECT_EQ(
            scrollOf(fixture.context().state(), ScreenRegion::OwnWaters).x,
            PANEL_SCROLL_STEP
        );
        EXPECT_EQ(fixture.context().state().placement.preferredShipLength, lengthBefore);
    }

    TEST(BattleInputTests, TypingLFoldsTheLogOnlyWhereItCanFold) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        fixture.game().play().shuffleFleet();
        fixture.game().play().beginBattle();
        BattleInput input{fixture.context()};
        const bool wasCollapsed = fixture.context().state().battle.isLogCollapsed;

        EXPECT_FALSE(input.interpret(typed("l")).has_value());
        EXPECT_EQ(fixture.context().state().battle.isLogCollapsed, wasCollapsed);

        fixture.context().geometry().rememberFoldable(ScreenRegion::Log, true);
        EXPECT_FALSE(input.interpret(typed("l")).has_value());
        EXPECT_EQ(fixture.context().state().battle.isLogCollapsed, !wasCollapsed);
    }

    TEST(BattleInputTests, ClickingTheLogHeadingFoldsOrOpensIt) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        fixture.game().play().shuffleFleet();
        fixture.game().play().beginBattle();
        BattleInput input{fixture.context()};
        fixture.context().geometry().rememberFoldable(ScreenRegion::Log, true);
        fixture.context().geometry().rememberPanel(
            ScreenRegion::Log,
            PanelExtent{.left = 1, .top = 20, .width = 40, .height = 0}
        );
        const bool wasCollapsed = fixture.context().state().battle.isLogCollapsed;
        const Keystroke tapOnHeading{
            .key = Key::Pointer,
            .button = PointerButton::Left,
            .isPressed = true,
            .pointerX = 5,
            .pointerY = 19
        };

        EXPECT_FALSE(input.interpret(tapOnHeading).has_value());

        EXPECT_EQ(fixture.context().state().battle.isLogCollapsed, !wasCollapsed);
    }

    TEST(PanelInputTests, ClickingALegendLinePressesItsKey) {
        PresentationFixture fixture;
        MenuInput input{fixture.context()};
        fixture.context().state().menu.selectedBoardSize = 12;
        fixture.context().geometry().rememberPanel(
            ScreenRegion::Shortcuts,
            PanelExtent{.left = 0, .top = 0, .width = 30, .height = 5}
        );
        fixture.context().geometry().rememberHotspots({KeyHotspot{
            .area = {.left = 0, .top = 0, .right = 29, .bottom = 0},
            .stroke = {.key = Key::Enter}
        }});
        const Keystroke click{
            .key = Key::Pointer,
            .button = PointerButton::Left,
            .isPressed = true,
            .pointerX = 10,
            .pointerY = 0
        };

        const std::optional<model::events::GameEvent> event = input.interpret(click);

        ASSERT_TRUE(event.has_value());
        ASSERT_TRUE(model::events::isKind<model::events::MatchStartRequested>(*event));
        EXPECT_EQ(std::get<model::events::MatchStartRequested>(*event).boardSize, 12);
    }

}  // namespace cpp_warships::head::common::input
