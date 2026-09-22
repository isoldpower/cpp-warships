#pragma once

#include <vector>

#include <game_tui/EventHandler.h>

namespace cpp_warships::game_tui {
    /** @brief Offers an event to its handlers in order, stopping at the first to claim it.
     *  Adding a way to respond to input means adding a handler, not editing a branch. */
    class EventRouter {
    public:
        void add(EventHandlerPointer handler);

        /** @brief Gives @p event to the first handler that claims it.
         *  @return whether any handler took it. */
        bool dispatch(ftxui::Event event);

    private:
        std::vector<EventHandlerPointer> handlers_;
    };
} // namespace cpp_warships::game_tui
