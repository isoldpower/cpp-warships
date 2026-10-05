#include <application/head/common/PresentationContext.h>
#include <application/head/common/input/Keystroke.h>
#include <application/head/common/input/PanelScrolling.h>
#include <application/head/common/input/keys/PanelKeys.h>

#include <algorithm>
#include <map>
#include <optional>

namespace cpp_warships::head::common::input::keys {
    namespace {
        [[nodiscard]] std::optional<FocusDirection> directionOf(const Keystroke& stroke) {
            static const std::map<Key, FocusDirection> DIRECTIONS = {
                {Key::ShiftArrowUp, FocusDirection::Up},
                {Key::ShiftArrowDown, FocusDirection::Down},
                {Key::ShiftArrowLeft, FocusDirection::Left},
                {Key::ShiftArrowRight, FocusDirection::Right},
            };

            const auto direction = DIRECTIONS.find(stroke.key);
            if (direction == DIRECTIONS.end()) {
                return std::nullopt;
            }

            return direction->second;
        }

        /** @brief How many lines one page of @p panel is: all of what shows but one line,
         * kept so the reader does not lose their place. */
        [[nodiscard]] int pageOf(const PresentationContext& context, const ScreenRegion panel) {
            const std::optional<PanelExtent> extent = context.geometry().panelOf(panel);
            if (!extent.has_value()) {
                return PANEL_SCROLL_STEP;
            }

            return std::max(1, extent->height - 1);
        }

        /** @brief Scrolls @p panel @p lines towards the top of what it holds, or towards the
         * bottom when @p lines is negative, reading back through a newest-first panel. */
        void scrollTowardsTop(PresentationContext& context, const ScreenRegion panel, int lines) {
            const int down = isNewestFirst(panel) ? lines : -lines;
            scrollPanel(context, panel, state::ScrollOffset{.x = 0, .y = down});
        }
    }  // namespace

    PanelKey::PanelKey(PresentationContext& context, const ScreenKind screen) noexcept
        : context_(context)
        , screen_(screen) {}

    bool MovePanelFocusKey::matches(const Keystroke& stroke) const {
        return directionOf(stroke).has_value();
    }

    std::optional<model::events::GameEvent> MovePanelFocusKey::interpret(const Keystroke& stroke) {
        const ScreenRegion from = focusedPanel(context_.state(), screen_);
        const std::optional<ScreenRegion> next =
            neighbourOf(context_.geometry(), screen_, from, *directionOf(stroke));

        if (next.has_value()) {
            context_.state().panels.focused[screen_] = *next;
        }

        return std::nullopt;
    }

    bool ScrollFocusedPanelKey::matches(const Keystroke& stroke) const {
        return stroke.key == Key::PageUp || stroke.key == Key::PageDown;
    }

    std::optional<model::events::GameEvent> ScrollFocusedPanelKey::interpret(
        const Keystroke& stroke
    ) {
        const ScreenRegion panel = focusedPanel(context_.state(), screen_);
        const int page = pageOf(context_, panel);

        scrollTowardsTop(context_, panel, stroke.key == Key::PageUp ? page : -page);
        return std::nullopt;
    }

    bool ScrollPanelWithWheelKey::matches(const Keystroke& stroke) const {
        if (!isWheelRolled(stroke)) {
            return false;
        }

        const ScreenRegion panel = context_.geometry().panelAt(stroke.pointerX, stroke.pointerY);
        const std::vector<ScreenRegion>& panels = panelsOf(screen_);
        return std::find(panels.begin(), panels.end(), panel) != panels.end();
    }

    std::optional<model::events::GameEvent> ScrollPanelWithWheelKey::interpret(
        const Keystroke& stroke
    ) {
        const ScreenRegion panel = context_.geometry().panelAt(stroke.pointerX, stroke.pointerY);
        const bool isTowardsStart =
            stroke.button == PointerButton::WheelUp || stroke.button == PointerButton::WheelLeft;

        if (isWheelRolledAcross(stroke)) {
            const int across = isTowardsStart ? -PANEL_SCROLL_STEP : PANEL_SCROLL_STEP;
            scrollPanel(context_, panel, state::ScrollOffset{.x = across, .y = 0});
            return std::nullopt;
        }

        scrollTowardsTop(context_, panel, isTowardsStart ? PANEL_SCROLL_STEP : -PANEL_SCROLL_STEP);
        return std::nullopt;
    }

    PressKeyHintKey::PressKeyHintKey(PresentationContext& context, ScreenInput& screen) noexcept
        : context_(context)
        , screen_(screen) {}

    std::optional<Keystroke> PressKeyHintKey::pressedBy(const Keystroke& stroke) const {
        if (!isPointer(stroke) || !stroke.isPressed || stroke.button != PointerButton::Left) {
            return std::nullopt;
        }

        return context_.geometry().hotspotAt(stroke.pointerX, stroke.pointerY);
    }

    bool PressKeyHintKey::matches(const Keystroke& stroke) const {
        return pressedBy(stroke).has_value();
    }

    std::optional<model::events::GameEvent> PressKeyHintKey::interpret(const Keystroke& stroke) {
        return screen_.interpret(*pressedBy(stroke));
    }
}  // namespace cpp_warships::head::common::input::keys
