#include <application/core/Board.h>
#include <application/core/Coordinate.h>
#include <application/head/common/PresentationContext.h>
#include <application/head/common/input/Keystroke.h>
#include <application/head/common/input/PanelScrolling.h>
#include <application/head/common/input/keys/MoveCursorKey.h>

#include <algorithm>
#include <map>

namespace cpp_warships::head::common::input::keys {
    namespace {
        const std::map<Key, core::Coordinate>& stepsByArrow() {
            static const std::map<Key, core::Coordinate> STEPS = {
                {Key::ArrowLeft, {-1, 0}},
                {Key::ArrowRight, {1, 0}},
                {Key::ArrowUp, {0, -1}},
                {Key::ArrowDown, {0, 1}},
            };
            return STEPS;
        }
    }  // namespace

    MoveCursorKey::MoveCursorKey(PresentationContext& context) noexcept
        : context_(context) {}

    bool MoveCursorKey::matches(const Keystroke& stroke) const {
        return stepsByArrow().contains(stroke.key);
    }

    std::optional<model::events::GameEvent> MoveCursorKey::interpret(const Keystroke& stroke) {
        const core::Board& walked = board();
        const core::Coordinate step = stepsByArrow().at(stroke.key);
        core::Coordinate& walking = cursor();

        walking.x = std::clamp(walking.x + step.x, 0, walked.width() - 1);
        walking.y = std::clamp(walking.y + step.y, 0, walked.height() - 1);
        revealCell(context_, region(), walking);

        return std::nullopt;
    }
}  // namespace cpp_warships::head::common::input::keys
