#include <application/head/common/PresentationContext.h>
#include <application/head/common/input/Keystroke.h>
#include <application/head/common/input/keys/BattleKeys.h>

namespace cpp_warships::head::common::input::keys {
    namespace {
        void toggleLog(PresentationContext& context) {
            bool& isCollapsed = context.state().battle.isLogCollapsed;
            isCollapsed = !isCollapsed;
        }
    }  // namespace

    BattleKey::BattleKey(PresentationContext& context) noexcept
        : context_(context) {}

    std::optional<core::Coordinate> BattleKey::cellUnderPointer(const Keystroke& stroke) const {
        if (!isPointer(stroke)) {
            return std::nullopt;
        }

        return context_.geometry()
            .cellAt(ScreenRegion::EnemyWaters, stroke.pointerX, stroke.pointerY);
    }

    core::Coordinate& MoveBattleAimKey::cursor() {
        return context_.state().battle.target;
    }

    const core::Board& MoveBattleAimKey::board() const {
        return context_.game().match().computerBoard();
    }

    ScreenRegion MoveBattleAimKey::region() const {
        return ScreenRegion::EnemyWaters;
    }

    bool FireKey::matches(const Keystroke& stroke) const {
        return stroke.key == Key::Enter;
    }

    std::optional<model::events::GameEvent> FireKey::interpret(const Keystroke&) {
        context_.state().battle.logScroll = 0;
        return model::events::ShotRequested{.target = context_.state().battle.target};
    }

    bool UseSkillKey::matches(const Keystroke& stroke) const {
        return isCharacter(stroke, "k");
    }

    std::optional<model::events::GameEvent> UseSkillKey::interpret(const Keystroke&) {
        context_.state().battle.logScroll = 0;
        return model::events::SkillUseRequested{.aim = context_.state().battle.target};
    }

    bool ToggleLogKey::matches(const Keystroke& stroke) const {
        return isCharacter(stroke, "l") && context_.geometry().isFoldable(ScreenRegion::Log);
    }

    std::optional<model::events::GameEvent> ToggleLogKey::interpret(const Keystroke&) {
        toggleLog(context_);
        return std::nullopt;
    }

    bool ToggleLogWithPointerKey::matches(const Keystroke& stroke) const {
        const std::optional<PanelExtent> log = context_.geometry().panelOf(ScreenRegion::Log);
        if (!log.has_value() || !context_.geometry().isFoldable(ScreenRegion::Log) ||
            !isPointer(stroke) || !stroke.isPressed || stroke.button != PointerButton::Left) {
            return false;
        }

        const bool isOnHeadingRow = stroke.pointerY == log->top - 1;
        return isOnHeadingRow && stroke.pointerX >= log->left &&
               stroke.pointerX < log->left + log->width;
    }

    std::optional<model::events::GameEvent> ToggleLogWithPointerKey::interpret(const Keystroke&) {
        toggleLog(context_);
        return std::nullopt;
    }

    bool FireWithPointerKey::matches(const Keystroke& stroke) const {
        return cellUnderPointer(stroke).has_value() && stroke.isPressed &&
               stroke.button == PointerButton::Left;
    }

    std::optional<model::events::GameEvent> FireWithPointerKey::interpret(const Keystroke& stroke) {
        const core::Coordinate cell = *cellUnderPointer(stroke);
        context_.state().battle.target = cell;
        context_.state().battle.logScroll = 0;

        return model::events::ShotRequested{.target = cell};
    }

    bool AimWithPointerAtEnemyKey::matches(const Keystroke& stroke) const {
        return cellUnderPointer(stroke).has_value();
    }

    std::optional<model::events::GameEvent> AimWithPointerAtEnemyKey::interpret(
        const Keystroke& stroke
    ) {
        context_.state().battle.target = *cellUnderPointer(stroke);
        return std::nullopt;
    }
}  // namespace cpp_warships::head::common::input::keys
