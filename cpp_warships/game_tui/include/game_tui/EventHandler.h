#pragma once

#include <memory>

#include <ftxui/component/event.hpp>

namespace cpp_warships::game_tui {
    /** @brief One thing a screen can do in response to input.
     *  The handler itself decides whether an event is any of its business. */
    class EventHandler {
    public:
        virtual ~EventHandler() = default;

        /** @brief Whether @p event is one this handler acts on. */
        [[nodiscard]] virtual bool isHandled(ftxui::Event event) const = 0;

        /** @brief Acts on @p event, having already claimed it through isHandled. */
        virtual void handleEvent(ftxui::Event event) = 0;
    };

    using EventHandlerPointer = std::shared_ptr<EventHandler>;
} // namespace cpp_warships::game_tui
