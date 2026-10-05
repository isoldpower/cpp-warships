#include <application/head/common/PresentationContext.h>
#include <application/head/common/render/Notices.h>
#include <application/head/plain/PlainFrame.h>
#include <application/head/plain/PlainSaveBrowserView.h>
#include <application/persistence/SaveArchive.h>

#include <cstddef>
#include <string>
#include <vector>

namespace cpp_warships::head::plain {
    namespace {
        [[nodiscard]] std::vector<std::string> saveLines(
            const std::vector<persistence::SaveSummary>& saves,
            const int chosen
        ) {
            if (saves.empty()) {
                return {"  nothing has been saved yet"};
            }

            std::vector<std::string> lines;
            for (std::size_t index = 0; index < saves.size(); ++index) {
                const std::string marker = static_cast<int>(index) == chosen ? "  > " : "    ";
                lines.push_back(
                    marker + persistence::SaveArchive::momentOf(saves[index].timestamp) + "   " +
                    saves[index].name
                );
            }

            return lines;
        }

        [[nodiscard]] std::vector<PlainKey> saveBrowserKeys(const bool hasSaves) {
            std::vector<PlainKey> keys{{"arrows", "choose a save"}};
            if (hasSaves) {
                keys.emplace_back("enter", "load it");
                keys.emplace_back("d", "delete it");
            }

            keys.emplace_back("esc", "back to the menu");
            return keys;
        }
    }  // namespace

    PlainSaveBrowserView::PlainSaveBrowserView(const common::PresentationContext& context) noexcept
        : context_(context) {}

    common::render::Frame PlainSaveBrowserView::render(int, int) {
        const std::vector<persistence::SaveSummary> saves = context_.game().saves().savedMatches();

        return PlainPage{}
            .lines(plainBanner("         SAVED GAMES          "))
            .blank()
            .lines(saveLines(saves, context_.state().saves.selectedIndex))
            .blank()
            .keys(saveBrowserKeys(!saves.empty()))
            .notices(context_.application())
            .frame();
    }
}  // namespace cpp_warships::head::plain
