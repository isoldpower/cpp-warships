#pragma once

#include <game_flow/Participant.h>

namespace cpp_warships::game_flow {
    /** @brief The other side of the board from @p participant. */
    [[nodiscard]] Participant opponentOf(Participant participant);

    /** @brief Whose turn it is to fire. */
    class TurnOrder {
    public:
        explicit TurnOrder(Participant startingParticipant = Participant::Player);

        [[nodiscard]] Participant current() const noexcept;
        [[nodiscard]] bool isPlayerTurn() const noexcept;

        /** @brief Hands the turn to the other participant. */
        void pass();

        /** @brief Hands the turn to @p participant, whoever holds it now. */
        void giveTo(Participant participant);

    private:
        Participant current_;
    };
} // namespace cpp_warships::game_flow
