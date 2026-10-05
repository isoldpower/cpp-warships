#include <application/core/Coordinate.h>
#include <application/head/common/PresentationContext.h>
#include <application/head/common/ScreenKind.h>
#include <application/head/common/input/GridGeometry.h>
#include <application/head/common/input/PanelScrolling.h>
#include <application/model/ApplicationContext.h>
#include <application/model/WarshipsGame.h>
#include <application/persistence/MemorySaveStorage.h>
#include <application/persistence/SaveArchive.h>
#include <gtest/gtest.h>

#include <optional>

namespace cpp_warships::head::common::input {
    namespace {
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

        private:
            flow::RandomEngine randomEngine_{17U};
            persistence::MemorySaveStorage storage_;
            persistence::SaveArchive archive_;
            model::WarshipsGame game_;
            model::ApplicationContext application_;
            PresentationContext context_;
        };

        /** @brief A panel window @p height lines tall over @p contentHeight lines of content. */
        PanelExtent windowOver(int left, int top, int height, int contentHeight) {
            constexpr int WIDTH = 20;
            return PanelExtent{
                .left = left,
                .top = top,
                .width = WIDTH,
                .height = height,
                .contentWidth = WIDTH,
                .contentHeight = contentHeight
            };
        }
    }  // namespace

    TEST(PanelScrollingTests, EveryScreenStartsWithItsFirstPanelFocused) {
        const state::PresentationState state;

        EXPECT_EQ(focusedPanel(state, ScreenKind::Battle), ScreenRegion::Log);
        EXPECT_EQ(focusedPanel(state, ScreenKind::Placement), ScreenRegion::OwnWaters);
        EXPECT_EQ(focusedPanel(state, ScreenKind::Menu), ScreenRegion::Settings);
        EXPECT_EQ(focusedPanel(state, ScreenKind::Saves), ScreenRegion::SaveList);
        EXPECT_EQ(focusedPanel(state, ScreenKind::SaveNaming), ScreenRegion::NameEntry);
    }

    TEST(PanelScrollingTests, AFocusOnAPanelTheScreenLacksFallsBackToTheFirst) {
        state::PresentationState state;
        state.panels.focused[ScreenKind::Menu] = ScreenRegion::Log;

        EXPECT_EQ(focusedPanel(state, ScreenKind::Menu), ScreenRegion::Settings);
    }

    TEST(PanelScrollingTests, ScrollingStopsAtEitherEndOfWhatAPanelHolds) {
        PresentationFixture fixture;
        fixture.context().geometry().rememberPanel(ScreenRegion::Skills, windowOver(0, 0, 5, 12));

        scrollPanel(fixture.context(), ScreenRegion::Skills, state::ScrollOffset{.x = 0, .y = 20});
        EXPECT_EQ(scrollOf(fixture.context().state(), ScreenRegion::Skills).y, 7);

        scrollPanel(fixture.context(), ScreenRegion::Skills, state::ScrollOffset{.x = 0, .y = -30});
        EXPECT_EQ(scrollOf(fixture.context().state(), ScreenRegion::Skills).y, 0);
    }

    TEST(PanelScrollingTests, TheLogScrollsThroughItsOwnRemembering) {
        PresentationFixture fixture;
        fixture.context().geometry().rememberPanel(ScreenRegion::Log, windowOver(0, 0, 4, 10));

        scrollPanel(fixture.context(), ScreenRegion::Log, state::ScrollOffset{.x = 0, .y = 3});

        EXPECT_EQ(fixture.context().state().battle.logScroll, 3);
        EXPECT_EQ(scrollOf(fixture.context().state(), ScreenRegion::Log).y, 3);
    }

    TEST(PanelScrollingTests, ALogNeverDrawnScrollsNoFurtherThanItsEntries) {
        PresentationFixture fixture;

        scrollPanel(fixture.context(), ScreenRegion::Log, state::ScrollOffset{.x = 0, .y = 5});

        EXPECT_EQ(fixture.context().state().battle.logScroll, 0);
    }

    TEST(PanelScrollingTests, AScrollLeftBehindByAShrunkPanelIsPulledBackFirst) {
        PresentationFixture fixture;
        fixture.context().state().panels.scrolled[ScreenRegion::Fleet] = {.x = 0, .y = 40};
        fixture.context().geometry().rememberPanel(ScreenRegion::Fleet, windowOver(0, 0, 5, 12));

        scrollPanel(fixture.context(), ScreenRegion::Fleet, state::ScrollOffset{.x = 0, .y = -1});

        EXPECT_EQ(scrollOf(fixture.context().state(), ScreenRegion::Fleet).y, 6);
    }

    TEST(PanelScrollingTests, RevealingScrollsAsLittleAsItTakes) {
        PresentationFixture fixture;
        fixture.context().geometry().rememberPanel(ScreenRegion::SaveList, windowOver(0, 0, 5, 20));
        const auto reveal = [&fixture](int line) {
            revealInPanel(
                fixture.context(),
                ScreenRegion::SaveList,
                ScreenArea{.left = 0, .top = line, .right = 0, .bottom = line}
            );
            return scrollOf(fixture.context().state(), ScreenRegion::SaveList).y;
        };

        EXPECT_EQ(reveal(7), 3);
        EXPECT_EQ(reveal(5), 3);
        EXPECT_EQ(reveal(2), 2);
    }

    TEST(PanelScrollingTests, RevealingACellBringsTheLabelsAlongOnTheFirstRow) {
        PresentationFixture fixture;
        GridGeometry& geometry = fixture.context().geometry();
        geometry.rememberBoard(ScreenRegion::OwnWaters, 3, -9, 39, 19, 10, 10, 4, 2);
        geometry.rememberPanel(
            ScreenRegion::OwnWaters,
            PanelExtent{
                .left = 0,
                .top = 0,
                .width = 42,
                .height = 5,
                .contentWidth = 42,
                .contentHeight = 20,
                .offsetX = 0,
                .offsetY = 10
            }
        );
        fixture.context().state().panels.scrolled[ScreenRegion::OwnWaters] = {.x = 0, .y = 10};

        revealCell(fixture.context(), ScreenRegion::OwnWaters, core::Coordinate{0, 0});
        EXPECT_EQ(scrollOf(fixture.context().state(), ScreenRegion::OwnWaters).y, 0);

        revealCell(fixture.context(), ScreenRegion::OwnWaters, core::Coordinate{0, 9});
        EXPECT_EQ(scrollOf(fixture.context().state(), ScreenRegion::OwnWaters).y, 15);
    }

    TEST(PanelScrollingTests, FindsTheNearestPanelInTheDirectionAsked) {
        GridGeometry geometry;
        geometry.rememberPanel(ScreenRegion::OwnWaters, windowOver(0, 0, 20, 20));
        geometry.rememberPanel(ScreenRegion::Fleet, windowOver(22, 0, 8, 8));
        geometry.rememberPanel(ScreenRegion::Shortcuts, windowOver(22, 10, 8, 8));

        const auto from = [&geometry](ScreenRegion panel, FocusDirection direction) {
            return neighbourOf(geometry, ScreenKind::Placement, panel, direction);
        };

        EXPECT_EQ(from(ScreenRegion::Fleet, FocusDirection::Down), ScreenRegion::Shortcuts);
        EXPECT_EQ(from(ScreenRegion::Shortcuts, FocusDirection::Up), ScreenRegion::Fleet);
        EXPECT_EQ(from(ScreenRegion::Shortcuts, FocusDirection::Left), ScreenRegion::OwnWaters);
        EXPECT_EQ(from(ScreenRegion::OwnWaters, FocusDirection::Left), std::nullopt);
    }

    TEST(PanelScrollingTests, APanelSqueezedToNothingCannotTakeTheFocus) {
        GridGeometry geometry;
        geometry.rememberPanel(ScreenRegion::Fleet, windowOver(0, 0, 8, 8));
        geometry.rememberPanel(ScreenRegion::Shortcuts, windowOver(0, 10, 0, 8));

        EXPECT_EQ(
            neighbourOf(geometry, ScreenKind::Placement, ScreenRegion::Fleet, FocusDirection::Down),
            std::nullopt
        );
    }

    TEST(PanelScrollingTests, OnlyTheLogListsItsNewestLineFirst) {
        EXPECT_TRUE(isNewestFirst(ScreenRegion::Log));
        EXPECT_FALSE(isNewestFirst(ScreenRegion::Skills));
        EXPECT_FALSE(isNewestFirst(ScreenRegion::OwnWaters));
    }
}  // namespace cpp_warships::head::common::input
