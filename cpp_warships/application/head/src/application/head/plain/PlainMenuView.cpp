#include <application/head/common/PresentationContext.h>
#include <application/head/common/render/Notices.h>
#include <application/head/plain/PlainFrame.h>
#include <application/head/plain/PlainMenuView.h>

#include <string>
#include <utility>
#include <vector>

namespace cpp_warships::head::plain {
    namespace {
        [[nodiscard]] std::vector<PlainKey> menuKeys(const model::WarshipsGame& game) {
            std::vector<PlainKey> keys{{"enter", "start a new match"}};
            if (game.hasMatch()) {
                keys.emplace_back("r", "resume the match in play");
                keys.emplace_back("s", "name it and quit");
            }
            if (!game.hasMatch() && game.saves().hasSavedMatch()) {
                keys.emplace_back("l", "load a saved match");
            }

            keys.emplace_back("q", "quit");
            return keys;
        }
    }  // namespace

    PlainMenuView::PlainMenuView(const common::PresentationContext& context) noexcept
        : context_(context) {}

    common::render::Frame PlainMenuView::render(int, int) {
        const std::string boardSize = std::to_string(context_.state().menu.selectedBoardSize);

        return PlainPage{}
            .lines(plainBanner("         CPP WARSHIPS         "))
            .blank()
            .line("  board size : " + boardSize + " x " + boardSize + "   (left / right)")
            .line("  theme      : " + context_.theme().name + "   (t)")
            .blank()
            .keys(menuKeys(context_.game()))
            .blank()
            .notices(context_.application())
            .frame();
    }
}  // namespace cpp_warships::head::plain
