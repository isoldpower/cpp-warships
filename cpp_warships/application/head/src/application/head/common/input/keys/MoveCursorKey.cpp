#include <application/core/Board.h>
#include <application/core/Coordinate.h>
#include <application/head/common/PresentationContext.h>
#include <application/head/common/input/Keystroke.h>
#include <application/head/common/input/PanelScrolling.h>
#include <application/head/common/input/keys/MoveCursorKey.h>

#include <algorithm>

namespace cpp_warships::head::common::input::keys {
    namespace {
        [[nodiscard]] core::Coordinate stepOf(const Keystroke& stroke) {
            switch (stroke.key) {
                case Key::ArrowLeft:
                    return {-1, 0};
                case Key::ArrowRight:
                    return {1, 0};
                case Key::ArrowUp:
                    return {0, -1};
                default:
                    return {0, 1};
            }
        }
    }  // namespace

    MoveCursorKey::MoveCursorKey(PresentationContext& context) noexcept
        : context_(context) {}

    bool MoveCursorKey::matches(const Keystroke& stroke) const {
        return stroke.key == Key::ArrowLeft || stroke.key == Key::ArrowRight ||
               stroke.key == Key::ArrowUp || stroke.key == Key::ArrowDown;
    }

    std::optional<model::events::GameEvent> MoveCursorKey::interpret(const Keystroke& stroke) {
        const core::Board& walked = board();
        const core::Coordinate step = stepOf(stroke);
        core::Coordinate& walking = cursor();

        walking.x = std::clamp(walking.x + step.x, 0, walked.width() - 1);
        walking.y = std::clamp(walking.y + step.y, 0, walked.height() - 1);
        revealCell(context_, region(), walking);

        return std::nullopt;
    }
}  // namespace cpp_warships::head::common::input::keys
