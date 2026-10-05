#include <application/head/common/PresentationContext.h>
#include <application/head/common/render/Notices.h>
#include <application/head/plain/PlainFrame.h>
#include <application/head/plain/PlainSaveNamingView.h>

#include <string>
#include <vector>

namespace cpp_warships::head::plain {
    namespace {
        /** @brief The name typed so far, and a word on why Enter waits while there is none. */
        [[nodiscard]] std::vector<std::string> namePrompt(const std::string& typed) {
            std::vector<std::string> lines{"  name: " + typed + "_", ""};
            if (typed.empty()) {
                lines.emplace_back("  a name is needed before it can be put away");
                lines.emplace_back("");
            }

            return lines;
        }

        [[nodiscard]] std::vector<PlainKey> namingKeys(const bool isNameEmpty) {
            std::vector<PlainKey> keys{{"letters", "type a name"}, {"back", "rub one out"}};
            if (!isNameEmpty) {
                keys.emplace_back("enter", "save and quit");
            }

            keys.emplace_back("esc", "back to the menu");
            return keys;
        }
    }  // namespace

    PlainSaveNamingView::PlainSaveNamingView(const common::PresentationContext& context) noexcept
        : context_(context) {}

    common::render::Frame PlainSaveNamingView::render(int, int) {
        const std::string& typed = context_.state().naming.typedName;

        return PlainPage{}
            .lines(plainBanner("       NAME THIS MATCH        "))
            .blank()
            .lines(namePrompt(typed))
            .keys(namingKeys(typed.empty()))
            .notices(context_.application())
            .frame();
    }
}  // namespace cpp_warships::head::plain
