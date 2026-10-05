#include <application/head/common/PresentationContext.h>
#include <application/head/tui/FtxuiNotices.h>
#include <application/head/tui/FtxuiPalette.h>
#include <application/head/tui/KeyHint.h>
#include <application/head/tui/SaveBrowserView.h>
#include <application/persistence/SaveArchive.h>

#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace cpp_warships::head::tui {
    namespace {
        constexpr int MINIMUM_PANEL_WIDTH = 46;
        ftxui::Element divider(const common::Theme& theme) {
            return ftxui::separator() | color(theme.border);
        }

        ftxui::Element sectionHeading(const common::Theme& theme, const std::string& title) {
            return ftxui::text(title) | ftxui::bold | color(theme.accent);
        }
    }  // namespace

    SaveBrowserView::SaveBrowserView(
        const common::PresentationContext& context,
        common::input::GridGeometry& geometry
    ) noexcept
        : context_(context)
        , geometry_(geometry)
        , bodyPanel_(common::input::ScreenRegion::SaveList)
        , shortcutsPanel_(common::input::ScreenRegion::Shortcuts) {}

    void SaveBrowserView::publishLayout() {
        bodyPanel_.publish(geometry_);
        shortcutsPanel_.publish(geometry_);
        hotspots_.publish(geometry_);
    }

    ftxui::Element SaveBrowserView::renderElement() {
        const common::Theme& theme = context_.theme();
        const std::vector<persistence::SaveSummary> saves = context_.game().saves().savedMatches();
        const int chosen = context_.state().saves.selectedIndex;
        constexpr common::ScreenKind SCREEN = common::ScreenKind::Saves;

        std::vector<ftxui::Element> rows;
        if (saves.empty()) {
            rows.push_back(ftxui::text("nothing has been saved yet") | color(theme.textMuted));
        }

        for (std::size_t index = 0; index < saves.size(); ++index) {
            const bool isChosen = static_cast<int>(index) == chosen;
            ftxui::Element line = ftxui::text(
                " " + persistence::SaveArchive::momentOf(saves[index].timestamp) + "   " +
                saves[index].name + " "
            );

            rows.push_back(
                isChosen ? std::move(line) | bgcolor(theme.accent) | color(theme.background)
                         : std::move(line) | color(theme.text)
            );
        }

        std::vector<KeyHint> hints{KeyHint{.key = "arrows", .description = "choose a save"}};
        if (!saves.empty()) {
            hints.push_back(KeyHint{.key = "enter", .description = "load it"});
            hints.push_back(KeyHint{.key = "d", .description = "delete it"});
        }
        hints.push_back(KeyHint{.key = "esc", .description = "back to the menu"});

        return dialogFrame(
            theme,
            ftxui::vbox(
                {bodyPanel_.render(
                     context_,
                     SCREEN,
                     sectionHeading(theme, "SAVED GAMES"),
                     ftxui::vbox(std::move(rows))
                 ),
                 divider(theme),
                 shortcutsPanel_.render(
                     context_,
                     SCREEN,
                     sectionHeading(theme, "KEYS"),
                     keyLegend(theme, std::move(hints), hotspots_)
                 ),
                 noticeBlock(theme, context_.application())}
            ),
            MINIMUM_PANEL_WIDTH,
            isNarrow()
        );
    }
}  // namespace cpp_warships::head::tui
