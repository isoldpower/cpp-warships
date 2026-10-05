#include <application/core/Board.h>
#include <application/flow/AiOpponent.h>
#include <application/flow/AttackOutcomeBehaviour.h>

#include <algorithm>
#include <array>
#include <functional>
#include <limits>
#include <utility>

namespace cpp_warships::flow {
    AiOpponent::AiOpponent(RandomEngine& randomEngine)
        : randomEngine_(randomEngine) {}

    AiOpponent::AiOpponent(RandomEngine& randomEngine, AiMemory memory)
        : randomEngine_(randomEngine)
        , attemptedCoordinates_(std::move(memory.attemptedCoordinates))
        , currentTargetHits_(std::move(memory.currentTargetHits)) {}

    AiMemory AiOpponent::memory() const {
        return {
            .attemptedCoordinates = attemptedCoordinates_,
            .currentTargetHits = currentTargetHits_
        };
    }

    std::optional<core::Coordinate> AiOpponent::unfinishedHit(const core::Board& board) const {
        const auto isStillHolding = [&board](const core::Coordinate& hit) {
            const auto cellState = board.stateAt(hit, core::Visibility::Opponent);
            return cellState == core::CellState::Damaged;
        };

        const auto damagedCell =
            std::find_if(currentTargetHits_.begin(), currentTargetHits_.end(), isStillHolding);

        if (damagedCell == currentTargetHits_.end()) {
            return std::nullopt;
        } else {
            return std::optional{*damagedCell};
        }
    }

    std::vector<core::Coordinate> AiOpponent::untriedNeighbours(
        core::Coordinate coordinate,
        const core::Board& board
    ) const {
        const std::array<core::Coordinate, 4> candidates{
            core::Coordinate{coordinate.x - 1, coordinate.y},
            core::Coordinate{coordinate.x + 1, coordinate.y},
            core::Coordinate{coordinate.x, coordinate.y - 1},
            core::Coordinate{coordinate.x, coordinate.y + 1}
        };

        std::vector<core::Coordinate> neighbours;
        for (const core::Coordinate& candidate : candidates) {
            if (board.contains(candidate) && !attemptedCoordinates_.contains(candidate)) {
                neighbours.push_back(candidate);
            }
        }

        return neighbours;
    }

    namespace {
        /** @brief The two cells just past either end of @p hits, which lie in one straight run. */
        std::array<core::Coordinate, 2> endsOfRun(const std::vector<core::Coordinate>& hits) {
            const bool isVerticalRun = hits[0].x == hits[1].x;
            const auto alongRun = [isVerticalRun](const core::Coordinate& hit) {
                return isVerticalRun ? hit.y : hit.x;
            };
            const auto onRun = [isVerticalRun,
                                fixedAxis = isVerticalRun ? hits[0].x : hits[0].y](int along) {
                return isVerticalRun ? core::Coordinate{fixedAxis, along}
                                     : core::Coordinate{along, fixedAxis};
            };

            const auto [lowest, highest] = std::minmax_element(
                hits.begin(),
                hits.end(),
                [&alongRun](const core::Coordinate& left, const core::Coordinate& right) {
                    return alongRun(left) < alongRun(right);
                }
            );

            return {onRun(alongRun(*lowest) - 1), onRun(alongRun(*highest) + 1)};
        }
    }  // namespace

    std::optional<core::Coordinate> AiOpponent::continueAlongHits(const core::Board& board) const {
        constexpr std::size_t HITS_THAT_MAKE_A_RUN = 2;
        if (currentTargetHits_.size() < HITS_THAT_MAKE_A_RUN) {
            return std::nullopt;
        }

        for (const core::Coordinate& extension : endsOfRun(currentTargetHits_)) {
            if (board.contains(extension) && !attemptedCoordinates_.contains(extension)) {
                return extension;
            }
        }

        return std::nullopt;
    }

    std::optional<core::Coordinate> AiOpponent::pickRandomUntried(const core::Board& board) {
        std::vector<core::Coordinate> available;
        for (int row = 0; row < board.height(); ++row) {
            for (int column = 0; column < board.width(); ++column) {
                const core::Coordinate coordinate{column, row};
                if (!attemptedCoordinates_.contains(coordinate)) {
                    available.push_back(coordinate);
                }
            }
        }

        if (available.empty()) {
            return std::nullopt;
        }

        std::uniform_int_distribution<std::size_t> distribution{0, available.size() - 1};
        return available[distribution(randomEngine_)];
    }

    std::optional<core::Coordinate> AiOpponent::chooseTarget(const core::Board& board) {
        using TargetingStrategy = std::function<std::optional<core::Coordinate>()>;
        const std::array<TargetingStrategy, 4> strategiesInOrder{
            [this, &board] { return unfinishedHit(board); },
            [this, &board] { return besideTheOnlyHit(board); },
            [this, &board] { return continueAlongHits(board); },
            [this, &board] { return pickRandomUntried(board); },
        };

        for (const TargetingStrategy& strategy : strategiesInOrder) {
            if (const std::optional<core::Coordinate> target = strategy()) {
                return target;
            }
        }

        return std::nullopt;
    }

    std::optional<core::Coordinate> AiOpponent::besideTheOnlyHit(const core::Board& board) {
        if (currentTargetHits_.size() != 1) {
            return std::nullopt;
        }

        const std::vector<core::Coordinate> neighbours =
            untriedNeighbours(currentTargetHits_.front(), board);
        if (neighbours.empty()) {
            return std::nullopt;
        }

        std::uniform_int_distribution<std::size_t> distribution{0, neighbours.size() - 1};
        return neighbours[distribution(randomEngine_)];
    }

    void AiOpponent::finishHunt(const core::Board& board) {
        for (const core::Coordinate& hit : currentTargetHits_) {
            for (int rowOffset = -1; rowOffset <= 1; ++rowOffset) {
                for (int columnOffset = -1; columnOffset <= 1; ++columnOffset) {
                    const core::Coordinate neighbour{hit.x + columnOffset, hit.y + rowOffset};

                    if (board.contains(neighbour)) {
                        attemptedCoordinates_.insert(neighbour);
                    }
                }
            }
        }

        currentTargetHits_.clear();
    }

    void AiOpponent::markAttempted(core::Coordinate coordinate) {
        attemptedCoordinates_.insert(coordinate);
    }

    void AiOpponent::registerHit(core::Coordinate coordinate) {
        const auto known =
            std::find(currentTargetHits_.begin(), currentTargetHits_.end(), coordinate);

        if (known == currentTargetHits_.end()) {
            currentTargetHits_.push_back(coordinate);
        }
    }

    void AiOpponent::recordOutcome(
        const core::Coordinate coordinate,
        const core::AttackOutcome outcome,
        const core::Board& board
    ) {
        const auto cellState = board.stateAt(coordinate, core::Visibility::Opponent);
        const bool isStillHolding = cellState == core::CellState::Damaged;
        if (!isStillHolding) {
            markAttempted(coordinate);
        }

        behaviourFor(outcome).updateHunt(*this, coordinate, board);
    }
}  // namespace cpp_warships::flow
