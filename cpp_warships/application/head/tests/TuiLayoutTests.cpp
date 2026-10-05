#include <application/head/common/PresentationContext.h>
#include <application/head/common/ScreenKind.h>
#include <application/head/common/input/Keystroke.h>
#include <application/head/common/input/PanelScrolling.h>
#include <application/head/common/render/Frame.h>
#include <application/head/tui/BattleView.h>
#include <application/head/tui/FtxuiView.h>
#include <application/head/tui/KeyHint.h>
#include <application/head/tui/MenuView.h>
#include <application/head/tui/ScrollPanel.h>
#include <application/model/ApplicationContext.h>
#include <application/model/WarshipsGame.h>
#include <application/persistence/MemorySaveStorage.h>
#include <application/persistence/SaveArchive.h>
#include <gtest/gtest.h>

#include <cstddef>
#include <ftxui/component/event.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <optional>
#include <string>
#include <vector>

namespace cpp_warships::head::tui {
    namespace {
        constexpr int BOARD_SIZE = 10;
        constexpr int CONTENT_LINES = 10;
        constexpr int WINDOW_WIDTH = 20;

        /** @brief A battle under way, presented, all kept alive together. */
        class BattleFixture {
        public:
            BattleFixture()
                : archive_(storage_)
                , game_(randomEngine_, archive_)
                , application_(game_)
                , context_(application_) {
                game_.play().startNewMatch(BOARD_SIZE);
                game_.play().shuffleFleet();
                game_.play().beginBattle();
            }

            common::PresentationContext& context() noexcept {
                return context_;
            }

        private:
            flow::RandomEngine randomEngine_{17U};
            persistence::MemorySaveStorage storage_;
            persistence::SaveArchive archive_;
            model::WarshipsGame game_;
            model::ApplicationContext application_;
            common::PresentationContext context_;
        };

        ftxui::Element numberedLines() {
            std::vector<ftxui::Element> lines;
            for (int line = 0; line < CONTENT_LINES; ++line) {
                lines.push_back(ftxui::text("line " + std::to_string(line)));
            }

            return ftxui::vbox(std::move(lines));
        }

        std::vector<std::string> rowsOf(const ftxui::Screen& screen) {
            std::vector<std::string> rows;
            for (int row = 0; row < screen.dimy(); ++row) {
                std::string text;
                for (int column = 0; column < screen.dimx(); ++column) {
                    const std::string& glyph = screen.PixelAt(column, row).character;
                    text += glyph.empty() ? " " : glyph;
                }
                rows.push_back(text);
            }

            return rows;
        }

        std::vector<std::string> rowsOf(const common::render::Frame& frame) {
            std::vector<std::string> rows;
            for (int row = 0; row < frame.height(); ++row) {
                std::string text;
                for (int column = 0; column < frame.width(); ++column) {
                    text += frame.at(column, row).glyph;
                }
                rows.push_back(text);
            }

            return rows;
        }

        /** @brief The first row holding @p wanted, or minus one when none does. */
        int rowHolding(const std::vector<std::string>& rows, const std::string& wanted) {
            for (std::size_t row = 0; row < rows.size(); ++row) {
                if (rows[row].find(wanted) != std::string::npos) {
                    return static_cast<int>(row);
                }
            }

            return -1;
        }

        std::vector<std::string> drawn(ftxui::Element element, int width, int height) {
            ftxui::Screen screen = ftxui::Screen::Create(
                ftxui::Dimension::Fixed(width),
                ftxui::Dimension::Fixed(height)
            );
            ftxui::Render(screen, element);
            return rowsOf(screen);
        }

        const ScrollBarColors BAR_COLORS{
            .track = ftxui::Color::Default,
            .thumb = ftxui::Color::Default
        };
    }  // namespace

    TEST(TuiKeystrokeTests, ShiftedArrowsAreToldApartFromPlainOnes) {
        using common::input::Key;

        EXPECT_EQ(keystrokeOf(ftxui::Event::Special("\x1B[1;2A")).key, Key::ShiftArrowUp);
        EXPECT_EQ(keystrokeOf(ftxui::Event::Special("\x1B[1;2B")).key, Key::ShiftArrowDown);
        EXPECT_EQ(keystrokeOf(ftxui::Event::Special("\x1B[1;2C")).key, Key::ShiftArrowRight);
        EXPECT_EQ(keystrokeOf(ftxui::Event::Special("\x1B[1;2D")).key, Key::ShiftArrowLeft);
        EXPECT_EQ(keystrokeOf(ftxui::Event::ArrowUp).key, Key::ArrowUp);
    }

    TEST(ScrollableTests, ShowsTheContentFromWhereItIsScrolledTo) {
        ScrollLayout layout;

        const std::vector<std::string> rows = drawn(
            scrollable(numberedLines(), {.x = 0, .y = 3}, layout, BAR_COLORS),
            WINDOW_WIDTH,
            4
        );

        EXPECT_NE(rows[0].find("line 3"), std::string::npos);
        EXPECT_NE(rows[3].find("line 6"), std::string::npos);
        EXPECT_EQ(layout.offsetY, 3);
        EXPECT_EQ(layout.contentHeight, CONTENT_LINES);
        EXPECT_EQ(layout.window.y_max - layout.window.y_min + 1, 4);
    }

    TEST(ScrollableTests, NeverScrollsPastTheEnd) {
        ScrollLayout layout;

        const std::vector<std::string> rows = drawn(
            scrollable(numberedLines(), {.x = 0, .y = 50}, layout, BAR_COLORS),
            WINDOW_WIDTH,
            4
        );

        EXPECT_EQ(layout.offsetY, CONTENT_LINES - 4);
        EXPECT_NE(rows[3].find("line 9"), std::string::npos);
    }

    TEST(ScrollableTests, DrawsABarOnlyWhenTheContentOverflows) {
        ScrollLayout overflowing;
        ScrollLayout fitting;

        const std::vector<std::string> squeezed =
            drawn(scrollable(numberedLines(), {}, overflowing, BAR_COLORS), WINDOW_WIDTH, 4);
        const std::vector<std::string> roomy = drawn(
            scrollable(numberedLines(), {}, fitting, BAR_COLORS),
            WINDOW_WIDTH,
            CONTENT_LINES
        );

        EXPECT_NE(squeezed[0].find("┃"), std::string::npos);
        EXPECT_EQ(overflowing.window.x_max - overflowing.window.x_min + 1, WINDOW_WIDTH - 1);
        EXPECT_EQ(roomy[0].find("┃"), std::string::npos);
        EXPECT_EQ(fitting.window.x_max - fitting.window.x_min + 1, WINDOW_WIDTH);
    }

    TEST(ScrollableTests, GivesWayWhenRoomRunsShort) {
        ScrollLayout layout;

        const std::vector<std::string> rows = drawn(
            ftxui::vbox(
                {ftxui::text("above"),
                 scrollable(numberedLines(), {}, layout, BAR_COLORS),
                 ftxui::text("below")}
            ),
            WINDOW_WIDTH,
            6
        );

        EXPECT_NE(rows[0].find("above"), std::string::npos);
        EXPECT_NE(rows[5].find("below"), std::string::npos);
        EXPECT_EQ(layout.window.y_max - layout.window.y_min + 1, 4);
    }

    TEST(BattleLayoutTests, StandsTheBoardsSideBySideOnAWideScreen) {
        BattleFixture fixture;
        BattleView view{fixture.context(), fixture.context().geometry()};

        const std::vector<std::string> rows = rowsOf(view.render(120, 40));

        EXPECT_NE(rowHolding(rows, "YOUR WATERS"), -1);
        EXPECT_EQ(rowHolding(rows, "YOUR WATERS"), rowHolding(rows, "ENEMY WATERS"));
    }

    TEST(BattleLayoutTests, StacksEveryPanelInOneColumnBelowTheBreakpoint) {
        BattleFixture fixture;
        BattleView view{fixture.context(), fixture.context().geometry()};

        const std::vector<std::string> rows = rowsOf(view.render(NARROW_LAYOUT_COLUMNS - 1, 60));

        const int enemy = rowHolding(rows, "ENEMY WATERS");
        const int own = rowHolding(rows, "YOUR WATERS");
        const int skills = rowHolding(rows, "SKILLS");
        const int story = rowHolding(rows, "LOG");
        const int keys = rowHolding(rows, "KEYS");
        EXPECT_NE(enemy, -1);
        EXPECT_LT(enemy, own);
        EXPECT_LT(own, skills);
        EXPECT_LT(skills, story);
        EXPECT_LT(story, keys);
    }

    TEST(BattleLayoutTests, WritesDownWhereEveryPanelLanded) {
        BattleFixture fixture;
        BattleView view{fixture.context(), fixture.context().geometry()};

        (void)view.render(NARROW_LAYOUT_COLUMNS - 1, 30);

        for (
            const common::input::ScreenRegion panel :
            common::input::panelsOf(common::ScreenKind::Battle)
        ) {
            EXPECT_TRUE(fixture.context().geometry().panelOf(panel).has_value());
        }
    }

    TEST(BattleLayoutTests, MarksThePanelThatHoldsTheFocus) {
        BattleFixture fixture;
        BattleView view{fixture.context(), fixture.context().geometry()};
        fixture.context().state().panels.focused[common::ScreenKind::Battle] =
            common::input::ScreenRegion::Skills;

        const std::vector<std::string> rows = rowsOf(view.render(120, 40));

        EXPECT_NE(rowHolding(rows, "▸ SKILLS"), -1);
        EXPECT_EQ(rowHolding(rows, "▸ LOG"), -1);
    }

    TEST(BattleLayoutTests, KeepsTheSidePanelToThirtyPercentOfTheScreenAtMost) {
        constexpr int SCREEN_WIDTH = LAPTOP_LAYOUT_COLUMNS + 20;
        constexpr int MOST_THE_SIDE_PANEL_MAY_TAKE = SCREEN_WIDTH * 30 / 100;
        BattleFixture fixture;
        BattleView view{fixture.context(), fixture.context().geometry()};

        (void)view.render(SCREEN_WIDTH, 40);

        const std::optional<common::input::PanelExtent> skills =
            fixture.context().geometry().panelOf(common::input::ScreenRegion::Skills);
        ASSERT_TRUE(skills.has_value());
        EXPECT_LE(SCREEN_WIDTH - skills->left, MOST_THE_SIDE_PANEL_MAY_TAKE + 1);
    }

    TEST(BattleLayoutTests, PutsTheSideBarUnderTheBoardsOnALaptopScreen) {
        BattleFixture fixture;
        BattleView view{fixture.context(), fixture.context().geometry()};

        const std::vector<std::string> rows = rowsOf(view.render(LAPTOP_LAYOUT_COLUMNS - 1, 40));

        const int boards = rowHolding(rows, "ENEMY WATERS");
        const int strip = rowHolding(rows, "SKILLS");
        EXPECT_NE(boards, -1);
        EXPECT_EQ(rowHolding(rows, "YOUR WATERS"), boards);
        EXPECT_GT(strip, boards);
        EXPECT_EQ(rowHolding(rows, "LOG"), strip);
        EXPECT_EQ(rowHolding(rows, "KEYS"), strip);
    }

    TEST(BattleLayoutTests, KeepsTheSideBarBesideTheBoardsFromTheLaptopBreakpointUp) {
        BattleFixture fixture;
        BattleView view{fixture.context(), fixture.context().geometry()};

        const std::vector<std::string> rows = rowsOf(view.render(LAPTOP_LAYOUT_COLUMNS, 40));

        EXPECT_EQ(rowHolding(rows, "SKILLS"), rowHolding(rows, "ENEMY WATERS"));
    }

    TEST(BattleLayoutTests, FoldsTheLogToItsHeadingOnANarrowScreen) {
        BattleFixture fixture;
        BattleView view{fixture.context(), fixture.context().geometry()};
        fixture.context().state().battle.isLogCollapsed = true;

        const std::vector<std::string> folded = rowsOf(view.render(NARROW_LAYOUT_COLUMNS - 1, 60));
        fixture.context().state().battle.isLogCollapsed = false;
        const std::vector<std::string> open = rowsOf(view.render(NARROW_LAYOUT_COLUMNS - 1, 60));

        EXPECT_NE(rowHolding(folded, "l show"), -1);
        EXPECT_EQ(rowHolding(folded, "nothing has happened yet"), -1);
        EXPECT_NE(rowHolding(open, "l hide"), -1);
        EXPECT_NE(rowHolding(open, "nothing has happened yet"), -1);
        EXPECT_TRUE(fixture.context().geometry().isFoldable(common::input::ScreenRegion::Log));
    }

    TEST(BattleLayoutTests, NeverFoldsTheLogWhereThereIsRoomForIt) {
        BattleFixture fixture;
        BattleView view{fixture.context(), fixture.context().geometry()};
        fixture.context().state().battle.isLogCollapsed = true;

        const std::vector<std::string> rows = rowsOf(view.render(LAPTOP_LAYOUT_COLUMNS, 40));

        EXPECT_NE(rowHolding(rows, "nothing has happened yet"), -1);
        EXPECT_EQ(rowHolding(rows, "l show"), -1);
        EXPECT_FALSE(fixture.context().geometry().isFoldable(common::input::ScreenRegion::Log));
    }

    TEST(KeyHintTests, OnlyAHintNamingOneKeyCanBePressed) {
        using common::input::Key;

        EXPECT_EQ(keystrokeOfHint("enter")->key, Key::Enter);
        EXPECT_EQ(keystrokeOfHint("esc")->key, Key::Escape);
        EXPECT_EQ(keystrokeOfHint("bksp")->key, Key::Backspace);
        EXPECT_EQ(keystrokeOfHint("k")->character, "k");
        EXPECT_EQ(keystrokeOfHint("arrows"), std::nullopt);
        EXPECT_EQ(keystrokeOfHint("shift arrows"), std::nullopt);
        EXPECT_EQ(keystrokeOfHint("pgup pgdn"), std::nullopt);
    }

    TEST(KeyHintTests, TheMenuLegendWritesDownWhereEachKeyCanBeClicked) {
        BattleFixture fixture;
        MenuView view{fixture.context(), fixture.context().geometry()};

        const std::vector<std::string> rows = rowsOf(view.render(120, 40));

        const int quitRow = rowHolding(rows, " q ");
        const int quitColumn =
            static_cast<int>(rows[static_cast<std::size_t>(quitRow)].find(" q "));
        const std::optional<common::input::Keystroke> pressed =
            fixture.context().geometry().hotspotAt(quitColumn + 1, quitRow);
        ASSERT_TRUE(pressed.has_value());
        EXPECT_EQ(pressed->character, "q");
    }

}  // namespace cpp_warships::head::tui
