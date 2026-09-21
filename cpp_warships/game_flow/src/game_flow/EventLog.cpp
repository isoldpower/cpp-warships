#include <game_flow/EventLog.h>

#include <utility>

namespace cpp_warships::game_flow {

    void EventLog::record(const MatchEvent& event) {
        entries_.push_back(event);
    }

    MatchEventLog EventLog::drain() {
        return std::exchange(entries_, MatchEventLog{});
    }

    const MatchEventLog& EventLog::entries() const noexcept {
        return entries_;
    }

    bool EventLog::isEmpty() const noexcept {
        return entries_.empty();
    }
} // namespace cpp_warships::game_flow
