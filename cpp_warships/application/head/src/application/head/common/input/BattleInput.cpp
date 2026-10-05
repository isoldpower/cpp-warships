#include <application/head/common/PresentationContext.h>
#include <application/head/common/input/BattleInput.h>
#include <application/head/common/input/keys/BattleKeys.h>
#include <application/head/common/input/keys/LeaveToMenuKey.h>
#include <application/head/common/input/keys/PanelKeys.h>

#include <memory>

namespace cpp_warships::head::common::input {
    BattleInput::BattleInput(PresentationContext& context) {
        bind(std::make_unique<keys::PressKeyHintKey>(context, *this));
        bind(std::make_unique<keys::MovePanelFocusKey>(context, ScreenKind::Battle));
        bind(std::make_unique<keys::ScrollFocusedPanelKey>(context, ScreenKind::Battle));
        bind(std::make_unique<keys::ScrollPanelWithWheelKey>(context, ScreenKind::Battle));
        bind(std::make_unique<keys::ToggleLogWithPointerKey>(context));
        bind(std::make_unique<keys::FireWithPointerKey>(context));
        bind(std::make_unique<keys::AimWithPointerAtEnemyKey>(context));
        bind(std::make_unique<keys::MoveBattleAimKey>(context));
        bind(std::make_unique<keys::FireKey>(context));
        bind(std::make_unique<keys::UseSkillKey>(context));
        bind(std::make_unique<keys::ToggleLogKey>(context));
        bind(std::make_unique<keys::LeaveToMenuKey>());
    }
}  // namespace cpp_warships::head::common::input
