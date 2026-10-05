#include <application/head/common/PresentationContext.h>
#include <application/head/common/input/SaveBrowserInput.h>
#include <application/head/common/input/keys/LeaveToMenuKey.h>
#include <application/head/common/input/keys/PanelKeys.h>
#include <application/head/common/input/keys/SaveBrowserKeys.h>

#include <memory>

namespace cpp_warships::head::common::input {
    SaveBrowserInput::SaveBrowserInput(PresentationContext& context) {
        bind(std::make_unique<keys::PressKeyHintKey>(context, *this));
        bind(std::make_unique<keys::MovePanelFocusKey>(context, ScreenKind::Saves));
        bind(std::make_unique<keys::ScrollFocusedPanelKey>(context, ScreenKind::Saves));
        bind(std::make_unique<keys::ScrollPanelWithWheelKey>(context, ScreenKind::Saves));
        bind(std::make_unique<keys::MoveSaveSelectionKey>(context));
        bind(std::make_unique<keys::LoadSelectedSaveKey>(context));
        bind(std::make_unique<keys::DeleteSelectedSaveKey>(context));
        bind(std::make_unique<keys::LeaveToMenuKey>());
    }
}  // namespace cpp_warships::head::common::input
