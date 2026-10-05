#include <application/head/common/PresentationContext.h>
#include <application/head/tui/FtxuiPalette.h>
#include <application/head/tui/SaveBrowserView.h>
#include <application/persistence/SaveArchive.h>

#include <cstddef>
#include <string>
#include <vector>

namespace cpp_warships::head::tui {
    namespace {
        /** @brief One save as a line of the list: when it was made and what it is called. */
        ftxui::Element saveLine(
            const common::Theme& theme,
            const persistence::SaveSummary& save,
            const bool isChosen
        ) {
            ftxui::Element line = ftxui::text(
                " " + persistence::SaveArchive::momentOf(save.timestamp) + "   " + save.name + " "
            );

            return isChosen ? std::move(line) | bgcolor(theme.accent) | color(theme.background)
                            : std::move(line) | color(theme.text);
        }
    }  // namespace

    SaveBrowserView::SaveBrowserView(
        const common::PresentationContext& context,
        common::input::GridGeometry& geometry
    ) noexcept
        : DialogView(
              context,
              geometry,
              common::ScreenKind::Saves,
              common::input::ScreenRegion::SaveList
          ) {}

    std::string SaveBrowserView::contentHeading() const {
        return "SAVED GAMES";
    }

    ftxui::Element SaveBrowserView::content() {
        const common::Theme& theme = context_.theme();
        const std::vector<persistence::SaveSummary> saves = context_.game().saves().savedMatches();
        if (saves.empty()) {
            return ftxui::text("nothing has been saved yet") | color(theme.textMuted);
        }

        const auto chosen = static_cast<std::size_t>(context_.state().saves.selectedIndex);
        std::vector<ftxui::Element> lines;
        for (std::size_t index = 0; index < saves.size(); ++index) {
            lines.push_back(saveLine(theme, saves[index], index == chosen));
        }

        return ftxui::vbox(std::move(lines));
    }

    std::vector<KeyHint> SaveBrowserView::hints() const {
        std::vector<KeyHint> listed{{.key = "arrows", .description = "choose a save"}};
        if (context_.game().saves().hasSavedMatch()) {
            listed.push_back({.key = "enter", .description = "load it"});
            listed.push_back({.key = "d", .description = "delete it"});
        }

        listed.push_back({.key = "esc", .description = "back to the menu"});
        return listed;
    }
}  // namespace cpp_warships::head::tui
