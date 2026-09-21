#include <game_flow/TurnOrder.h>

namespace cpp_warships::game_flow {

    Participant opponentOf(Participant participant) {
        return participant == Participant::Player ? Participant::Computer : Participant::Player;
    }

    TurnOrder::TurnOrder(Participant startingParticipant)
        : current_(startingParticipant) {}

    Participant TurnOrder::current() const noexcept {
        return current_;
    }

    bool TurnOrder::isPlayerTurn() const noexcept {
        return current_ == Participant::Player;
    }

    void TurnOrder::pass() {
        current_ = opponentOf(current_);
    }

    void TurnOrder::giveTo(Participant participant) {
        current_ = participant;
    }
} // namespace cpp_warships::game_flow
