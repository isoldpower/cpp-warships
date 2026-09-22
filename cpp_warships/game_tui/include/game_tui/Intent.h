#pragma once

#include <functional>
#include <memory>

namespace cpp_warships::game_tui {
    class Application;

    /** @brief Something the player has asked for, as a value a component can hand upwards.
     *  Components emit these; only an intent is allowed to change the application. */
    class Intent {
    public:
        virtual ~Intent() = default;

        virtual void applyTo(Application& application) const = 0;
    };

    using IntentPointer = std::shared_ptr<const Intent>;

    /** @brief Where a component sends the intents it raises. */
    using IntentSink = std::function<void(IntentPointer)>;
} // namespace cpp_warships::game_tui
