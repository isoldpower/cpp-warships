#pragma once

#include <map>

namespace cpp_warships::game_core {

    class FleetComposition {
    public:
        FleetComposition() = default;
        explicit FleetComposition(std::map<int, int> countsByLength);

        [[nodiscard]] static FleetComposition forBoardSize(int boardSize);

        [[nodiscard]] const std::map<int, int>& countsByLength() const noexcept;
        [[nodiscard]] int countOf(int length) const;
        [[nodiscard]] int totalShips() const;
        [[nodiscard]] int totalCells() const;
        [[nodiscard]] bool isEmpty() const noexcept;

    private:
        std::map<int, int> countsByLength_;
    };
} // namespace cpp_warships::game_core
