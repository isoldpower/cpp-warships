#include <application/head/common/PresentationContext.h>
#include <application/head/tui/FtxuiPalette.h>
#include <application/head/tui/SaveNamingView.h>

#include <string>
#include <vector>

namespace cpp_warships::head::tui {
    SaveNamingView::SaveNamingView(
        const common::PresentationContext& context,
        common::input::GridGeometry& geometry
    ) noexcept
        : DialogView(
              context,
              geometry,
              common::ScreenKind::SaveNaming,
              common::input::ScreenRegion::NameEntry
          ) {}

    std::string SaveNamingView::contentHeading() const {
        return "NAME THIS MATCH";
    }

    ftxui::Element SaveNamingView::content() {
        const common::Theme& theme = context_.theme();
        const std::string& typed = context_.state().naming.typedName;
        const ftxui::Element nameLine = ftxui::hbox(
            {ftxui::text("name  ") | color(theme.textMuted),
             ftxui::text(typed + "_") | ftxui::bold | color(theme.text)}
        );

        if (!typed.empty()) {
            return nameLine;
        }

        return ftxui::vbox(
            {nameLine,
             ftxui::text("a name is needed before it can be put away") | color(theme.textMuted)}
        );
    }

    std::vector<KeyHint> SaveNamingView::hints() const {
        std::vector<KeyHint> listed{
            {.key = "letters", .description = "type a name"},
            {.key = "back", .description = "rub one out"},
        };
        if (!context_.state().naming.typedName.empty()) {
            listed.push_back({.key = "enter", .description = "save and quit"});
        }

        listed.push_back({.key = "esc", .description = "back to the menu"});
        return listed;
    }
}  // namespace cpp_warships::head::tui
