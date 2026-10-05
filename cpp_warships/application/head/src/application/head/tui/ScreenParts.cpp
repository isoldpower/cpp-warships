#include <application/head/tui/FtxuiPalette.h>
#include <application/head/tui/ScreenParts.h>

namespace cpp_warships::head::tui {
    ftxui::Element divider(const common::Theme& theme) {
        return ftxui::separator() | color(theme.border);
    }

    ftxui::Element sectionHeading(const common::Theme& theme, const std::string& title) {
        return ftxui::text(title) | ftxui::bold | color(theme.accent);
    }
}  // namespace cpp_warships::head::tui
