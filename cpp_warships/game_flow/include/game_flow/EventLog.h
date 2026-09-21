#pragma once

#include <game_flow/MatchEvent.h>

namespace cpp_warships::game_flow {
    /** @brief The running history of a match, written by the model and read by the interface.
     *  Draining hands the entries over and starts a fresh page. */
    class EventLog {
    public:
        void record(const MatchEvent& event);

        /** @brief Returns everything recorded since the last call and clears the log. */
        MatchEventLog drain();

        [[nodiscard]] const MatchEventLog& entries() const noexcept;
        [[nodiscard]] bool isEmpty() const noexcept;

    private:
        MatchEventLog entries_;
    };
} // namespace cpp_warships::game_flow
