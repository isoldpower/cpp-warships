#include <application/core/Board.h>
#include <application/core/Coordinate.h>
#include <application/core/Direction.h>
#include <application/core/Outcomes.h>
#include <application/core/Segment.h>
#include <application/core/Ship.h>
#include <gtest/gtest.h>

#include <unordered_set>
#include <utility>
#include <vector>

namespace cpp_warships::core {
    namespace {
        constexpr int BOARD_SIZE = 10;

        Board makeBoard() {
            return Board{BOARD_SIZE, BOARD_SIZE};
        }

        /** @brief Attacks every cell of @p ship until it goes down. */
        void sinkShipAt(
            Board& board,
            const Coordinate origin,
            const Direction direction,
            const int length
        ) {
            const Ship outline{origin, direction, length};
            for (const Coordinate coordinate : outline.coordinates()) {
                while (
                    board.stateAt(coordinate, Visibility::Owner) != CellState::Destroyed &&
                    board.stateAt(coordinate, Visibility::Owner) != CellState::Sunk
                ) {
                    board.attack(coordinate, 1);
                }
            }
        }
    }  // namespace

    TEST(BoardTests, ReportsItsSize) {
        const Board board{8, 12};

        EXPECT_EQ(board.width(), 8);
        EXPECT_EQ(board.height(), 12);
    }

    TEST(BoardTests, ClampsANegativeSizeToZero) {
        const Board board{-4, -1};

        EXPECT_EQ(board.width(), 0);
        EXPECT_EQ(board.height(), 0);
        EXPECT_FALSE(board.contains({0, 0}));
    }

    TEST(BoardTests, ContainsOnlyCellsInsideItsBounds) {
        const Board board = makeBoard();

        EXPECT_TRUE(board.contains({0, 0}));
        EXPECT_TRUE(board.contains({9, 9}));
        EXPECT_FALSE(board.contains({-1, 0}));
        EXPECT_FALSE(board.contains({0, -1}));
        EXPECT_FALSE(board.contains({10, 0}));
        EXPECT_FALSE(board.contains({0, 10}));
    }

    TEST(BoardTests, StartsEmpty) {
        const Board board = makeBoard();

        EXPECT_FALSE(board.hasShips());
        EXPECT_TRUE(board.ships().empty());
        EXPECT_TRUE(board.attackedCells().empty());
    }

    TEST(BoardTests, PlacesAShipThatFits) {
        Board board = makeBoard();

        EXPECT_EQ(board.place({0, 0}, Direction::Horizontal, 3), PlacementError::None);
        EXPECT_TRUE(board.hasShips());
        EXPECT_EQ(board.ships().size(), 1U);
        EXPECT_EQ(board.ships().front().length(), 3);
    }

    TEST(BoardTests, RejectsAShipOfNonPositiveLength) {
        Board board = makeBoard();

        EXPECT_EQ(board.canPlace({0, 0}, Direction::Horizontal, 0), PlacementError::InvalidLength);
        EXPECT_EQ(board.canPlace({0, 0}, Direction::Horizontal, -2), PlacementError::InvalidLength);
        EXPECT_EQ(board.place({0, 0}, Direction::Horizontal, 0), PlacementError::InvalidLength);
        EXPECT_FALSE(board.hasShips());
    }

    TEST(BoardTests, RejectsAShipRunningOffTheBoard) {
        Board board = makeBoard();

        EXPECT_EQ(board.place({8, 0}, Direction::Horizontal, 3), PlacementError::OutOfBounds);
        EXPECT_EQ(board.place({0, 8}, Direction::Vertical, 3), PlacementError::OutOfBounds);
        EXPECT_EQ(board.place({-1, 0}, Direction::Horizontal, 2), PlacementError::OutOfBounds);
        EXPECT_FALSE(board.hasShips());
    }

    TEST(BoardTests, RejectsAShipOverlappingAnother) {
        Board board = makeBoard();
        board.place({4, 4}, Direction::Horizontal, 3);

        EXPECT_EQ(board.place({4, 4}, Direction::Vertical, 2), PlacementError::Overlaps);
        EXPECT_EQ(board.ships().size(), 1U);
    }

    TEST(BoardTests, RejectsAShipTouchingAnotherSideOn) {
        Board board = makeBoard();
        board.place({4, 4}, Direction::Horizontal, 2);

        EXPECT_EQ(
            board.place({4, 5}, Direction::Horizontal, 2),
            PlacementError::TouchesAnotherShip
        );
        EXPECT_EQ(board.ships().size(), 1U);
    }

    TEST(BoardTests, RejectsAShipTouchingAnotherCornerOn) {
        Board board = makeBoard();
        board.place({4, 4}, Direction::Horizontal, 2);

        EXPECT_EQ(
            board.place({6, 5}, Direction::Horizontal, 2),
            PlacementError::TouchesAnotherShip
        );
    }

    TEST(BoardTests, AcceptsAShipOneClearCellAway) {
        Board board = makeBoard();
        board.place({4, 4}, Direction::Horizontal, 2);

        EXPECT_EQ(board.place({4, 6}, Direction::Horizontal, 2), PlacementError::None);
        EXPECT_EQ(board.ships().size(), 2U);
    }

    TEST(BoardTests, LeavesTheBoardUntouchedWhenPlacementFails) {
        Board board = makeBoard();
        board.place({0, 0}, Direction::Horizontal, 2);

        board.place({0, 1}, Direction::Horizontal, 2);

        EXPECT_EQ(board.ships().size(), 1U);
    }

    TEST(BoardTests, RemovesTheShipCoveringACell) {
        Board board = makeBoard();
        board.place({2, 2}, Direction::Horizontal, 3);

        EXPECT_TRUE(board.removeShipAt({3, 2}));
        EXPECT_FALSE(board.hasShips());
    }

    TEST(BoardTests, RefusesToRemoveWhereThereIsNoShip) {
        Board board = makeBoard();
        board.place({2, 2}, Direction::Horizontal, 3);

        EXPECT_FALSE(board.removeShipAt({7, 7}));
        EXPECT_EQ(board.ships().size(), 1U);
    }

    TEST(BoardTests, AttackingEmptyWaterMisses) {
        Board board = makeBoard();

        EXPECT_EQ(board.attack({5, 5}, 1), AttackOutcome::Miss);
        EXPECT_TRUE(board.attackedCells().contains({5, 5}));
    }

    TEST(BoardTests, AttackingTheSameWaterTwiceIsRefused) {
        Board board = makeBoard();
        board.attack({5, 5}, 1);

        EXPECT_EQ(board.attack({5, 5}, 1), AttackOutcome::AlreadyAttacked);
    }

    TEST(BoardTests, AttackingOffTheBoardIsRefused) {
        Board board = makeBoard();

        EXPECT_EQ(board.attack({-1, 0}, 1), AttackOutcome::OutOfBounds);
        EXPECT_EQ(board.attack({10, 10}, 1), AttackOutcome::OutOfBounds);
        EXPECT_TRUE(board.attackedCells().empty());
    }

    TEST(BoardTests, AttackingAShipHitsWhileASegmentSurvives) {
        Board board = makeBoard();
        board.place({0, 0}, Direction::Horizontal, 2, 2);

        EXPECT_EQ(board.attack({0, 0}, 1), AttackOutcome::Hit);
        EXPECT_EQ(board.attack({0, 0}, 1), AttackOutcome::Hit);
    }

    TEST(BoardTests, ASegmentAlreadyDestroyedCannotBeAttackedAgain) {
        Board board = makeBoard();
        board.place({0, 0}, Direction::Horizontal, 2, 1);
        board.attack({0, 0}, 1);

        EXPECT_EQ(board.attack({0, 0}, 1), AttackOutcome::AlreadyAttacked);
    }

    TEST(BoardTests, TheFinalSegmentSinksTheShip) {
        Board board = makeBoard();
        board.place({0, 0}, Direction::Horizontal, 2, 1);
        board.attack({0, 0}, 1);

        EXPECT_EQ(board.attack({1, 0}, 1), AttackOutcome::Sunk);
        EXPECT_TRUE(board.ships().front().isSunk());
    }

    TEST(BoardTests, DamageCarriesThroughToTheSegment) {
        Board board = makeBoard();
        board.place({0, 0}, Direction::Horizontal, 1, 5);

        EXPECT_EQ(board.attack({0, 0}, 3), AttackOutcome::Hit);
        EXPECT_EQ(board.ships().front().segmentHealth(0), 2);
    }

    TEST(BoardTests, SinkingAShipRevealsTheWaterHuggingIt) {
        Board board = makeBoard();
        board.place({4, 4}, Direction::Horizontal, 1, 1);

        board.attack({4, 4}, 1);

        EXPECT_TRUE(board.attackedCells().contains({3, 3}));
        EXPECT_TRUE(board.attackedCells().contains({5, 5}));
        EXPECT_TRUE(board.attackedCells().contains({4, 3}));
        EXPECT_FALSE(board.attackedCells().contains({4, 2}));
    }

    TEST(BoardTests, RevealedWaterStaysInsideTheBoard) {
        Board board = makeBoard();
        board.place({0, 0}, Direction::Horizontal, 1, 1);

        board.attack({0, 0}, 1);

        for (const Coordinate cell : board.attackedCells()) {
            EXPECT_TRUE(board.contains(cell));
        }
    }

    TEST(BoardTests, RevealingWaterNeverMarksAnotherShip) {
        Board board = makeBoard();
        board.place({4, 4}, Direction::Horizontal, 1, 1);
        board.place({4, 6}, Direction::Horizontal, 1, 1);

        board.attack({4, 4}, 1);

        EXPECT_FALSE(board.attackedCells().contains({4, 6}));
    }

    TEST(BoardTests, OwnerSeesShipsThatTheOpponentCannot) {
        Board board = makeBoard();
        board.place({2, 2}, Direction::Horizontal, 2);

        EXPECT_EQ(board.stateAt({2, 2}, Visibility::Owner), CellState::Ship);
        EXPECT_EQ(board.stateAt({2, 2}, Visibility::Opponent), CellState::Water);
    }

    TEST(BoardTests, UntouchedWaterReadsAsWaterToBothSides) {
        const Board board = makeBoard();

        EXPECT_EQ(board.stateAt({7, 7}, Visibility::Owner), CellState::Water);
        EXPECT_EQ(board.stateAt({7, 7}, Visibility::Opponent), CellState::Water);
    }

    TEST(BoardTests, AttackedWaterReadsAsAMiss) {
        Board board = makeBoard();
        board.attack({7, 7}, 1);

        EXPECT_EQ(board.stateAt({7, 7}, Visibility::Opponent), CellState::Miss);
    }

    TEST(BoardTests, AHurtSegmentReadsAsDamaged) {
        Board board = makeBoard();
        board.place({0, 0}, Direction::Horizontal, 2, 2);
        board.attack({0, 0}, 1);

        EXPECT_EQ(board.stateAt({0, 0}, Visibility::Opponent), CellState::Damaged);
    }

    TEST(BoardTests, AFlattenedSegmentOfALiveShipReadsAsDestroyed) {
        Board board = makeBoard();
        board.place({0, 0}, Direction::Horizontal, 2, 1);
        board.attack({0, 0}, 1);

        EXPECT_EQ(board.stateAt({0, 0}, Visibility::Opponent), CellState::Destroyed);
    }

    TEST(BoardTests, EverySegmentOfASunkShipReadsAsSunk) {
        Board board = makeBoard();
        board.place({0, 0}, Direction::Horizontal, 2, 1);
        sinkShipAt(board, {0, 0}, Direction::Horizontal, 2);

        EXPECT_EQ(board.stateAt({0, 0}, Visibility::Opponent), CellState::Sunk);
        EXPECT_EQ(board.stateAt({1, 0}, Visibility::Opponent), CellState::Sunk);
    }

    TEST(BoardTests, FindsAShipWithinARadiusIgnoringFog) {
        Board board = makeBoard();
        board.place({5, 5}, Direction::Horizontal, 1);

        EXPECT_TRUE(board.hasShipWithin({5, 5}, 0));
        EXPECT_TRUE(board.hasShipWithin({7, 7}, 2));
        EXPECT_FALSE(board.hasShipWithin({8, 8}, 2));
    }

    TEST(BoardTests, ARadiusReachingOffTheBoardIsHarmless) {
        Board board = makeBoard();
        board.place({0, 0}, Direction::Horizontal, 1);

        EXPECT_TRUE(board.hasShipWithin({0, 0}, 3));
    }

    TEST(BoardTests, AnEmptyBoardIsNotALostOne) {
        const Board board = makeBoard();

        EXPECT_FALSE(board.allShipsSunk());
    }

    TEST(BoardTests, AllShipsSunkOnlyOnceTheLastOneGoesDown) {
        Board board = makeBoard();
        board.place({0, 0}, Direction::Horizontal, 1, 1);
        board.place({0, 2}, Direction::Horizontal, 1, 1);

        board.attack({0, 0}, 1);
        EXPECT_FALSE(board.allShipsSunk());

        board.attack({0, 2}, 1);
        EXPECT_TRUE(board.allShipsSunk());
    }

    TEST(BoardTests, ClearingDropsShipsAndShotsButKeepsTheSize) {
        Board board = makeBoard();
        board.place({0, 0}, Direction::Horizontal, 2);
        board.attack({5, 5}, 1);

        board.clear();

        EXPECT_FALSE(board.hasShips());
        EXPECT_TRUE(board.attackedCells().empty());
        EXPECT_EQ(board.width(), BOARD_SIZE);
        EXPECT_EQ(board.height(), BOARD_SIZE);
    }

    TEST(BoardTests, RebuildsFromShipsAndShotsAlreadyOnIt) {
        std::vector<Ship> ships;
        ships.emplace_back(Coordinate{1, 1}, Direction::Horizontal, 2, 1);
        ships.front().damageSegment(0, 1);
        const std::unordered_set<Coordinate> attacked{{1, 1}, {8, 8}};

        Board board{BOARD_SIZE, BOARD_SIZE, std::move(ships), attacked};

        EXPECT_EQ(board.ships().size(), 1U);
        EXPECT_EQ(board.attackedCells().size(), 2U);
        EXPECT_EQ(board.stateAt({1, 1}, Visibility::Opponent), CellState::Destroyed);
        EXPECT_EQ(board.stateAt({8, 8}, Visibility::Opponent), CellState::Miss);
        EXPECT_EQ(board.attack({2, 1}, 1), AttackOutcome::Sunk);
    }

    TEST(BoardTests, TellsTheOwnerTheHealthOfEverySegment) {
        Board board = makeBoard();
        board.place({0, 0}, Direction::Horizontal, 2, 3);

        EXPECT_EQ(board.healthAt({0, 0}, Visibility::Owner), 3);
        EXPECT_EQ(board.healthAt({5, 5}, Visibility::Owner), std::nullopt);
    }

    TEST(BoardTests, TellsTheOpponentTheHealthOfASegmentOnlyOnceItIsHit) {
        Board board = makeBoard();
        board.place({0, 0}, Direction::Horizontal, 2, 3);

        EXPECT_EQ(board.healthAt({0, 0}, Visibility::Opponent), std::nullopt);

        (void)board.attack({0, 0}, 1);

        EXPECT_EQ(board.healthAt({0, 0}, Visibility::Opponent), 2);
        EXPECT_EQ(board.healthAt({1, 0}, Visibility::Opponent), std::nullopt);
    }
}  // namespace cpp_warships::core
