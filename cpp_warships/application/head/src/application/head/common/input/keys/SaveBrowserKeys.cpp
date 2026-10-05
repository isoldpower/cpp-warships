#include <application/head/common/PresentationContext.h>
#include <application/head/common/input/Keystroke.h>
#include <application/head/common/input/PanelScrolling.h>
#include <application/head/common/input/keys/SaveBrowserKeys.h>

#include <algorithm>
#include <cstddef>
#include <vector>

namespace cpp_warships::head::common::input::keys {
    namespace {
        [[nodiscard]] int lastIndexOf(const PresentationContext& context) {
            return static_cast<int>(context.game().saves().savedMatches().size()) - 1;
        }
    }  // namespace

    std::string selectedSave(const PresentationContext& context) {
        const std::vector<persistence::SaveSummary> saves = context.game().saves().savedMatches();
        const int chosen = context.state().saves.selectedIndex;
        if (chosen < 0 || chosen >= static_cast<int>(saves.size())) {
            return {};
        }

        return saves[static_cast<std::size_t>(chosen)].id;
    }

    SaveBrowserKey::SaveBrowserKey(PresentationContext& context) noexcept
        : context_(context) {}

    bool MoveSaveSelectionKey::matches(const Keystroke& stroke) const {
        return stroke.key == Key::ArrowUp || stroke.key == Key::ArrowDown;
    }

    std::optional<model::events::GameEvent> MoveSaveSelectionKey::interpret(
        const Keystroke& stroke
    ) {
        const int step = stroke.key == Key::ArrowUp ? -1 : 1;
        int& chosen = context_.state().saves.selectedIndex;

        chosen = std::clamp(chosen + step, 0, std::max(0, lastIndexOf(context_)));
        revealInPanel(
            context_,
            ScreenRegion::SaveList,
            ScreenArea{.left = 0, .top = chosen, .right = 0, .bottom = chosen}
        );
        return std::nullopt;
    }

    bool LoadSelectedSaveKey::matches(const Keystroke& stroke) const {
        return stroke.key == Key::Enter && !selectedSave(context_).empty();
    }

    std::optional<model::events::GameEvent> LoadSelectedSaveKey::interpret(const Keystroke&) {
        return model::events::MatchLoadRequested{.name = selectedSave(context_)};
    }

    bool DeleteSelectedSaveKey::matches(const Keystroke& stroke) const {
        const bool isDeleteKey =
            stroke.key == Key::Backspace || stroke.key == Key::Delete || isCharacter(stroke, "d");

        return isDeleteKey && !selectedSave(context_).empty();
    }

    std::optional<model::events::GameEvent> DeleteSelectedSaveKey::interpret(const Keystroke&) {
        return model::events::SaveDeleteRequested{.name = selectedSave(context_)};
    }
}  // namespace cpp_warships::head::common::input::keys
