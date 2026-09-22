#include <game_tui/EventRouter.h>

#include <algorithm>
#include <utility>

namespace cpp_warships::game_tui {

    void EventRouter::add(EventHandlerPointer handler) {
        if (handler) {
            handlers_.push_back(std::move(handler));
        }
    }

    bool EventRouter::dispatch(ftxui::Event event) {
        const auto claimsEvent = [event](const EventHandlerPointer& handler) {
            return handler->isHandled(event);
        };

        const auto found = std::find_if(
            handlers_.begin(),
            handlers_.end(),
            claimsEvent
        );

        if (found == handlers_.end()) {
            return false;
        }
        (*found)->handleEvent(event);
        return true;
    }
} // namespace cpp_warships::game_tui
