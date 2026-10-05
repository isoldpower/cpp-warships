#pragma once

#include <application/head/common/input/InputKey.h>
#include <application/head/common/input/ScreenRegion.h>

namespace cpp_warships::core {
    class Board;
    struct Coordinate;
}  // namespace cpp_warships::core

namespace cpp_warships::head::common {
    class PresentationContext;
}

namespace cpp_warships::head::common::input::keys {
    /** @brief Walks a cursor around a board with the arrow keys, stopping at the edges and
     *  scrolling its panel to keep it in view. */
    class MoveCursorKey : public InputKey {
    public:
        explicit MoveCursorKey(PresentationContext& context) noexcept;

        [[nodiscard]] bool matches(const Keystroke& stroke) const final;
        [[nodiscard]] std::optional<model::events::GameEvent> interpret(
            const Keystroke& stroke
        ) final;

    protected:
        /** @brief The cursor this walks, which the screen it belongs to owns. */
        [[nodiscard]] virtual core::Coordinate& cursor() = 0;

        /** @brief The board it is walked around, whose edges stop it. */
        [[nodiscard]] virtual const core::Board& board() const = 0;

        /** @brief The panel the board is drawn in, scrolled to keep the cursor in view. */
        [[nodiscard]] virtual ScreenRegion region() const = 0;

        PresentationContext& context_;
    };
}  // namespace cpp_warships::head::common::input::keys
