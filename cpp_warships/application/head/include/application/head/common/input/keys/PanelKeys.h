#pragma once

#include <application/head/common/ScreenKind.h>
#include <application/head/common/input/InputKey.h>
#include <application/head/common/input/ScreenInput.h>

namespace cpp_warships::head::common {
    class PresentationContext;
}

namespace cpp_warships::head::common::input::keys {
    /** @brief A binding that works on the panels of one screen, whichever screen that is. */
    class PanelKey : public InputKey {
    public:
        PanelKey(PresentationContext& context, ScreenKind screen) noexcept;

    protected:
        PresentationContext& context_;
        ScreenKind screen_;
    };

    /** @brief Hands the focus to the nearest panel the shifted arrow points at. */
    class MovePanelFocusKey final : public PanelKey {
    public:
        using PanelKey::PanelKey;

        [[nodiscard]] bool matches(const Keystroke& stroke) const override;
        [[nodiscard]] std::optional<model::events::GameEvent> interpret(
            const Keystroke& stroke
        ) override;
    };

    /** @brief Pages through whichever panel holds the focus. */
    class ScrollFocusedPanelKey final : public PanelKey {
    public:
        using PanelKey::PanelKey;

        [[nodiscard]] bool matches(const Keystroke& stroke) const override;
        [[nodiscard]] std::optional<model::events::GameEvent> interpret(
            const Keystroke& stroke
        ) override;
    };

    /** @brief Scrolls whichever panel the wheel is rolled over. */
    class ScrollPanelWithWheelKey final : public PanelKey {
    public:
        using PanelKey::PanelKey;

        [[nodiscard]] bool matches(const Keystroke& stroke) const override;
        [[nodiscard]] std::optional<model::events::GameEvent> interpret(
            const Keystroke& stroke
        ) override;
    };

    /** @brief Presses whichever key the clicked line of the key legend names, by reading
     * that key through the same screen input it would have arrived at. */
    class PressKeyHintKey final : public InputKey {
    public:
        /** @brief Reads pressed keys through @p screen, which must outlive this binding. */
        PressKeyHintKey(PresentationContext& context, ScreenInput& screen) noexcept;

        [[nodiscard]] bool matches(const Keystroke& stroke) const override;
        [[nodiscard]] std::optional<model::events::GameEvent> interpret(
            const Keystroke& stroke
        ) override;

    private:
        [[nodiscard]] std::optional<Keystroke> pressedBy(const Keystroke& stroke) const;

        PresentationContext& context_;
        ScreenInput& screen_;
    };
}  // namespace cpp_warships::head::common::input::keys
