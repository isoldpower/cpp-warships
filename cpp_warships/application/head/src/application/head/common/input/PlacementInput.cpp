#include <application/head/common/PresentationContext.h>
#include <application/head/common/input/PlacementInput.h>
#include <application/head/common/input/keys/LeaveToMenuKey.h>
#include <application/head/common/input/keys/PanelKeys.h>
#include <application/head/common/input/keys/PlacementKeys.h>

#include <memory>

namespace cpp_warships::head::common::input {
    PlacementInput::PlacementInput(PresentationContext& context) {
        bind(std::make_unique<keys::PressKeyHintKey>(context, *this));
        bind(std::make_unique<keys::LayShipWithPointerKey>(context));
        bind(std::make_unique<keys::TakeShipBackWithPointerKey>(context));
        bind(std::make_unique<keys::MovePanelFocusKey>(context, ScreenKind::Placement));
        bind(std::make_unique<keys::ScrollFocusedPanelKey>(context, ScreenKind::Placement));
        bind(std::make_unique<keys::ScrollPanelWithWheelKey>(context, ScreenKind::Placement));
        bind(std::make_unique<keys::AimWithPointerKey>(context));
        bind(std::make_unique<keys::MovePlacementCursorKey>(context));
        bind(std::make_unique<keys::TurnShipKey>(context));
        bind(std::make_unique<keys::PickShipLengthKey>(context));
        bind(std::make_unique<keys::LayShipKey>(context));
        bind(std::make_unique<keys::TakeShipBackKey>(context));
        bind(std::make_unique<keys::ShuffleFleetKey>());
        bind(std::make_unique<keys::BeginBattleKey>());
        bind(std::make_unique<keys::LeaveToMenuKey>());
    }
}  // namespace cpp_warships::head::common::input
