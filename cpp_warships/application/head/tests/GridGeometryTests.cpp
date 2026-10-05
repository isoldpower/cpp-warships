#include <application/core/Coordinate.h>
#include <application/head/common/input/GridGeometry.h>
#include <gtest/gtest.h>

#include <optional>

namespace cpp_warships::head::common::input {
    namespace {
        constexpr int COLUMN_PITCH = 2;
        constexpr int ROW_PITCH = 1;

        /** @brief Remembers an 8x8 board drawn at (10, 5), two columns per cell. */
        void rememberEnemyBoard(GridGeometry& geometry) {
            geometry.rememberBoard(
                ScreenRegion::EnemyWaters,
                10,
                5,
                16,
                8,
                8,
                8,
                COLUMN_PITCH,
                ROW_PITCH
            );
        }
    }  // namespace

    TEST(GridGeometryTests, KnowsNothingUntilSomethingIsDrawn) {
        const GridGeometry geometry;

        EXPECT_EQ(geometry.regionAt(0, 0), ScreenRegion::Elsewhere);
        EXPECT_EQ(geometry.cellAt(ScreenRegion::EnemyWaters, 0, 0), std::nullopt);
    }

    TEST(GridGeometryTests, PlacesAPointerInTheRegionItIsOver) {
        GridGeometry geometry;
        rememberEnemyBoard(geometry);

        EXPECT_EQ(geometry.regionAt(10, 5), ScreenRegion::EnemyWaters);
        EXPECT_EQ(geometry.regionAt(25, 12), ScreenRegion::EnemyWaters);
    }

    TEST(GridGeometryTests, PlacesAPointerOutsideEveryRegionElsewhere) {
        GridGeometry geometry;
        rememberEnemyBoard(geometry);

        EXPECT_EQ(geometry.regionAt(9, 5), ScreenRegion::Elsewhere);
        EXPECT_EQ(geometry.regionAt(26, 5), ScreenRegion::Elsewhere);
        EXPECT_EQ(geometry.regionAt(10, 4), ScreenRegion::Elsewhere);
        EXPECT_EQ(geometry.regionAt(10, 13), ScreenRegion::Elsewhere);
    }

    TEST(GridGeometryTests, TurnsAPointerIntoTheCellUnderIt) {
        GridGeometry geometry;
        rememberEnemyBoard(geometry);

        EXPECT_EQ(geometry.cellAt(ScreenRegion::EnemyWaters, 10, 5), (core::Coordinate{0, 0}));
        EXPECT_EQ(geometry.cellAt(ScreenRegion::EnemyWaters, 12, 5), (core::Coordinate{1, 0}));
        EXPECT_EQ(geometry.cellAt(ScreenRegion::EnemyWaters, 10, 6), (core::Coordinate{0, 1}));
    }

    TEST(GridGeometryTests, EveryColumnOfACellAnswersAsThatCell) {
        GridGeometry geometry;
        rememberEnemyBoard(geometry);

        EXPECT_EQ(geometry.cellAt(ScreenRegion::EnemyWaters, 12, 5), (core::Coordinate{1, 0}));
        EXPECT_EQ(geometry.cellAt(ScreenRegion::EnemyWaters, 13, 5), (core::Coordinate{1, 0}));
    }

    TEST(GridGeometryTests, APointerOutsideThePatchIsOverNoCell) {
        GridGeometry geometry;
        rememberEnemyBoard(geometry);

        EXPECT_EQ(geometry.cellAt(ScreenRegion::EnemyWaters, 9, 5), std::nullopt);
        EXPECT_EQ(geometry.cellAt(ScreenRegion::EnemyWaters, 10, 20), std::nullopt);
    }

    TEST(GridGeometryTests, APatchWiderThanItsBoardStopsAtTheLastCell) {
        GridGeometry geometry;
        geometry.rememberBoard(ScreenRegion::OwnWaters, 0, 0, 20, 4, 4, 4, 2, 1);

        EXPECT_EQ(geometry.cellAt(ScreenRegion::OwnWaters, 6, 0), (core::Coordinate{3, 0}));
        EXPECT_EQ(geometry.cellAt(ScreenRegion::OwnWaters, 8, 0), std::nullopt);
    }

    TEST(GridGeometryTests, TellsTheTwoBoardsApart) {
        GridGeometry geometry;
        geometry.rememberBoard(ScreenRegion::OwnWaters, 0, 0, 8, 4, 4, 4, 2, 1);
        geometry.rememberBoard(ScreenRegion::EnemyWaters, 20, 0, 8, 4, 4, 4, 2, 1);

        EXPECT_EQ(geometry.regionAt(0, 0), ScreenRegion::OwnWaters);
        EXPECT_EQ(geometry.regionAt(20, 0), ScreenRegion::EnemyWaters);
        EXPECT_EQ(geometry.cellAt(ScreenRegion::OwnWaters, 2, 0), (core::Coordinate{1, 0}));
        EXPECT_EQ(geometry.cellAt(ScreenRegion::EnemyWaters, 22, 0), (core::Coordinate{1, 0}));
    }

    TEST(GridGeometryTests, RemembersWhereThePanelWithNoBoardWasDrawn) {
        GridGeometry geometry;
        geometry.rememberPanel(
            ScreenRegion::Log,
            PanelExtent{.left = 40, .top = 2, .width = 30, .height = 12}
        );

        EXPECT_EQ(geometry.regionAt(45, 6), ScreenRegion::Log);
        EXPECT_EQ(geometry.regionAt(39, 6), ScreenRegion::Elsewhere);
    }

    TEST(GridGeometryTests, TheLogHoldsNoCells) {
        GridGeometry geometry;
        geometry.rememberPanel(
            ScreenRegion::Log,
            PanelExtent{.left = 40, .top = 2, .width = 30, .height = 12}
        );

        EXPECT_EQ(geometry.cellAt(ScreenRegion::Log, 45, 6), std::nullopt);
    }

    TEST(GridGeometryTests, ClearingForgetsEverythingDrawn) {
        GridGeometry geometry;
        rememberEnemyBoard(geometry);
        geometry.rememberPanel(
            ScreenRegion::Log,
            PanelExtent{.left = 40, .top = 2, .width = 30, .height = 12}
        );

        geometry.clear();

        EXPECT_EQ(geometry.regionAt(10, 5), ScreenRegion::Elsewhere);
        EXPECT_EQ(geometry.regionAt(45, 6), ScreenRegion::Elsewhere);
    }

    TEST(GridGeometryTests, RememberingAgainReplacesWhereARegionWas) {
        GridGeometry geometry;
        rememberEnemyBoard(geometry);

        geometry.rememberBoard(ScreenRegion::EnemyWaters, 0, 0, 8, 4, 4, 4, 2, 1);

        EXPECT_EQ(geometry.regionAt(10, 5), ScreenRegion::Elsewhere);
        EXPECT_EQ(geometry.regionAt(0, 0), ScreenRegion::EnemyWaters);
    }

    TEST(GridGeometryTests, NothingIsEverOverTheElsewhereRegion) {
        GridGeometry geometry;
        rememberEnemyBoard(geometry);

        EXPECT_EQ(geometry.cellAt(ScreenRegion::Elsewhere, 10, 5), std::nullopt);
    }

    TEST(GridGeometryTests, APitchOfNothingYieldsNoCell) {
        GridGeometry geometry;
        geometry.rememberBoard(ScreenRegion::OwnWaters, 0, 0, 8, 4, 4, 4, 0, 1);

        EXPECT_EQ(geometry.cellAt(ScreenRegion::OwnWaters, 2, 0), std::nullopt);
    }

    TEST(GridGeometryTests, ABoardOnlyAnswersWhereItsPanelLeavesItShowing) {
        GridGeometry geometry;
        geometry
            .rememberBoard(ScreenRegion::EnemyWaters, 10, 3, 16, 8, 8, 8, COLUMN_PITCH, ROW_PITCH);
        geometry.rememberPanel(
            ScreenRegion::EnemyWaters,
            PanelExtent{.left = 10, .top = 5, .width = 16, .height = 4}
        );

        EXPECT_EQ(geometry.cellAt(ScreenRegion::EnemyWaters, 10, 4), std::nullopt);
        EXPECT_EQ(geometry.regionAt(10, 4), ScreenRegion::Elsewhere);
        EXPECT_EQ(geometry.cellAt(ScreenRegion::EnemyWaters, 10, 5), (core::Coordinate{0, 2}));
        EXPECT_EQ(geometry.cellAt(ScreenRegion::EnemyWaters, 10, 9), std::nullopt);
    }

    TEST(GridGeometryTests, FindsThePanelUnderAPointerBoardsIncluded) {
        GridGeometry geometry;
        geometry.rememberPanel(
            ScreenRegion::OwnWaters,
            PanelExtent{.left = 0, .top = 0, .width = 10, .height = 10}
        );
        geometry.rememberPanel(
            ScreenRegion::Skills,
            PanelExtent{.left = 20, .top = 0, .width = 10, .height = 10}
        );

        EXPECT_EQ(geometry.panelAt(1, 1), ScreenRegion::OwnWaters);
        EXPECT_EQ(geometry.regionAt(1, 1), ScreenRegion::Elsewhere);
        EXPECT_EQ(geometry.panelAt(21, 1), ScreenRegion::Skills);
        EXPECT_EQ(geometry.regionAt(21, 1), ScreenRegion::Skills);
        EXPECT_EQ(geometry.panelAt(15, 1), ScreenRegion::Elsewhere);
    }

    TEST(GridGeometryTests, KnowsWhereACellWasDrawn) {
        GridGeometry geometry;
        rememberEnemyBoard(geometry);

        const std::optional<ScreenArea> area =
            geometry.areaOfCell(ScreenRegion::EnemyWaters, core::Coordinate{1, 2});

        ASSERT_TRUE(area.has_value());
        EXPECT_EQ(area->left, 12);
        EXPECT_EQ(area->top, 7);
        EXPECT_EQ(area->right, 13);
        EXPECT_EQ(area->bottom, 7);
    }

    TEST(GridGeometryTests, ACellOffTheBoardTakesUpNoRoom) {
        GridGeometry geometry;
        rememberEnemyBoard(geometry);

        EXPECT_EQ(
            geometry.areaOfCell(ScreenRegion::EnemyWaters, core::Coordinate{8, 0}),
            std::nullopt
        );
        EXPECT_EQ(
            geometry.areaOfCell(ScreenRegion::OwnWaters, core::Coordinate{0, 0}),
            std::nullopt
        );
    }

    TEST(GridGeometryTests, ALegendLinePressesItsKeyOnlyWhereItsPanelShowsIt) {
        GridGeometry geometry;
        geometry.rememberPanel(
            ScreenRegion::Shortcuts,
            PanelExtent{.left = 50, .top = 10, .width = 20, .height = 2}
        );
        geometry.rememberHotspots(
            {KeyHotspot{
                 .area = {.left = 50, .top = 10, .right = 69, .bottom = 10},
                 .stroke = {.key = Key::Enter}
             },
             KeyHotspot{
                 .area = {.left = 50, .top = 13, .right = 69, .bottom = 13},
                 .stroke = {.key = Key::Escape}
             }}
        );

        ASSERT_TRUE(geometry.hotspotAt(55, 10).has_value());
        EXPECT_EQ(geometry.hotspotAt(55, 10)->key, Key::Enter);
        EXPECT_EQ(geometry.hotspotAt(55, 11), std::nullopt);
        EXPECT_EQ(geometry.hotspotAt(55, 13), std::nullopt);
    }

}  // namespace cpp_warships::head::common::input
