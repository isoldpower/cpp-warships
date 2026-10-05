#include <application/head/common/PresentationContext.h>
#include <application/head/common/input/SaveNamingInput.h>
#include <application/head/common/input/keys/LeaveToMenuKey.h>
#include <application/head/common/input/keys/PanelKeys.h>
#include <application/head/common/input/keys/SaveNamingKeys.h>

#include <memory>

namespace cpp_warships::head::common::input {
    SaveNamingInput::SaveNamingInput(PresentationContext& context) {
        bind(std::make_unique<keys::PressKeyHintKey>(context, *this));
        bind(std::make_unique<keys::MovePanelFocusKey>(context, ScreenKind::SaveNaming));
        bind(std::make_unique<keys::ScrollFocusedPanelKey>(context, ScreenKind::SaveNaming));
        bind(std::make_unique<keys::ScrollPanelWithWheelKey>(context, ScreenKind::SaveNaming));
        bind(std::make_unique<keys::ConfirmNameKey>(context));
        bind(std::make_unique<keys::EraseNameKey>(context));
        bind(std::make_unique<keys::LeaveToMenuKey>());
        bind(std::make_unique<keys::TypeNameKey>(context));
    }
}  // namespace cpp_warships::head::common::input
