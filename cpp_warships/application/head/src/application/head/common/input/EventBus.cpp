#include <application/head/common/input/EventBus.h>
#include <application/head/common/input/Keystroke.h>

#include <map>
#include <utility>

namespace cpp_warships::head::common::input {
    void EventBus::readScreenWith(const ScreenKind screen, std::unique_ptr<ScreenInput> input) {
        inputs_[screen] = std::move(input);
    }

    std::optional<model::events::GameEvent> EventBus::interpret(
        const ScreenKind screen,
        const Keystroke& stroke
    ) {
        const auto reader = inputs_.find(screen);
        if (reader == inputs_.end()) {
            return std::nullopt;
        }

        return reader->second->interpret(stroke);
    }

    model::events::EventScope scopeOf(const ScreenKind screen) noexcept {
        static const std::map<ScreenKind, model::events::EventScope> SCOPES = {
            {ScreenKind::Menu, model::events::EventScope::Menu},
            {ScreenKind::Saves, model::events::EventScope::Saves},
            {ScreenKind::SaveNaming, model::events::EventScope::SaveNaming},
            {ScreenKind::Placement, model::events::EventScope::Placement},
            {ScreenKind::Battle, model::events::EventScope::Battle},
        };

        const auto scope = SCOPES.find(screen);
        return scope == SCOPES.end() ? model::events::EventScope::Always : scope->second;
    }
}  // namespace cpp_warships::head::common::input
