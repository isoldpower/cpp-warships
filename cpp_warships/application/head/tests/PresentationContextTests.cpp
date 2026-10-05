#include <application/flow/MatchPhase.h>
#include <application/head/common/PresentationContext.h>
#include <application/head/common/ScreenKind.h>
#include <application/head/common/input/EventBus.h>
#include <application/model/ApplicationContext.h>
#include <application/model/WarshipsGame.h>
#include <application/model/events/EventScope.h>
#include <application/persistence/MemorySaveStorage.h>
#include <application/persistence/SaveArchive.h>
#include <gtest/gtest.h>

namespace cpp_warships::head::common {
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
            flow::RandomEngine randomEngine_{42U};
            persistence::MemorySaveStorage storage_;
            persistence::SaveArchive archive_;
            model::WarshipsGame game_;
            model::ApplicationContext application_;
            PresentationContext context_;
        };
    }  // namespace

    TEST(PresentationContextTests, StartsAtTheMenuOnTheDefaultTheme) {
        PresentationFixture fixture;

        EXPECT_EQ(fixture.context().currentScreen(), ScreenKind::Menu);
        EXPECT_EQ(fixture.context().theme().name, defaultTheme().name);
        EXPECT_TRUE(fixture.context().state().isAtMenu);
    }

    TEST(PresentationContextTests, ReadsTheGameItPresents) {
        PresentationFixture fixture;

        EXPECT_FALSE(fixture.context().game().hasMatch());
        EXPECT_EQ(&fixture.context().application().game(), &fixture.game());
    }

    TEST(PresentationContextTests, StaysAtTheMenuWhenThereIsNoMatch) {
        PresentationFixture fixture;
        fixture.context().state().isAtMenu = false;

        EXPECT_EQ(fixture.context().currentScreen(), ScreenKind::Menu);
    }

    TEST(PresentationContextTests, ShowsPlacementWhileAFleetIsBeingLaidOut) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        fixture.context().state().isAtMenu = false;

        EXPECT_EQ(fixture.context().currentScreen(), ScreenKind::Placement);
    }

    TEST(PresentationContextTests, ShowsBattleOnceTheFightingStarts) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        fixture.game().play().shuffleFleet();
        fixture.game().play().beginBattle();
        fixture.context().state().isAtMenu = false;

        EXPECT_EQ(fixture.context().currentScreen(), ScreenKind::Battle);
    }

    TEST(PresentationContextTests, TheMenuWinsOverAMatchInPlay) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        fixture.context().state().isAtMenu = true;

        EXPECT_EQ(fixture.context().currentScreen(), ScreenKind::Menu);
    }

    TEST(PresentationContextTests, BrowsingSavesWinsOverTheMenu) {
        PresentationFixture fixture;
        fixture.context().state().isBrowsingSaves = true;

        EXPECT_EQ(fixture.context().currentScreen(), ScreenKind::Saves);
    }

    TEST(PresentationContextTests, NamingASaveWinsOverEverything) {
        PresentationFixture fixture;
        fixture.game().play().startNewMatch(BOARD_SIZE);
        fixture.context().state().isAtMenu = false;
        fixture.context().state().isBrowsingSaves = true;
        fixture.context().state().isNamingSave = true;

        EXPECT_EQ(fixture.context().currentScreen(), ScreenKind::SaveNaming);
    }

    TEST(PresentationContextTests, ChangingTheThemeIsSeenThroughTheContext) {
        PresentationFixture fixture;
        const std::string wanted = availableThemes().back().name;

        fixture.context().themeSelection().change(wanted);

        EXPECT_EQ(fixture.context().theme().name, wanted);
    }

    TEST(PresentationContextTests, HoldsTheInterfacesOwnRemembering) {
        PresentationFixture fixture;

        fixture.context().state().battle.target = core::Coordinate{3, 4};
        fixture.context().geometry().rememberPanel(
            input::ScreenRegion::Log,
            input::PanelExtent{.left = 0, .top = 0, .width = 10, .height = 10}
        );

        EXPECT_EQ(fixture.context().state().battle.target, (core::Coordinate{3, 4}));
        EXPECT_EQ(fixture.context().geometry().regionAt(1, 1), input::ScreenRegion::Log);
    }

    TEST(EventBusTests, EveryScreenHasAScopeOfItsOwn) {
        EXPECT_EQ(input::scopeOf(ScreenKind::Menu), model::events::EventScope::Menu);
        EXPECT_EQ(input::scopeOf(ScreenKind::Saves), model::events::EventScope::Saves);
        EXPECT_EQ(input::scopeOf(ScreenKind::SaveNaming), model::events::EventScope::SaveNaming);
        EXPECT_EQ(input::scopeOf(ScreenKind::Placement), model::events::EventScope::Placement);
        EXPECT_EQ(input::scopeOf(ScreenKind::Battle), model::events::EventScope::Battle);
    }
}  // namespace cpp_warships::head::common
