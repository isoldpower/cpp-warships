#include <application/head/common/PresentationContext.h>
#include <application/head/common/render/CoordinateLabel.h>
#include <application/head/common/render/EventNarration.h>
#include <application/head/common/render/Notices.h>
#include <application/head/plain/PlainBattleView.h>
#include <application/head/plain/PlainBoardGrid.h>
#include <application/head/plain/PlainFrame.h>

#include <algorithm>
#include <cstddef>
#include <deque>
#include <string>
#include <utility>
#include <vector>

namespace cpp_warships::head::plain {
    namespace {
        [[nodiscard]] std::string skillBankLine(const flow::Match& match) {
            const std::deque<flow::SkillKind>& banked = match.skills().pending();
            if (banked.empty()) {
                return "SKILLS  none in the bank";
            }

            std::string line = "SKILLS  ";
            for (std::size_t position = 0; position < banked.size(); ++position) {
                line += position > 0 ? ", " : "";
                line += common::render::skillName(banked[position]);
            }

            return line;
        }

        /** @brief The window on the story, newest first and scrolled back the same way the
         * interactive log scrolls, so the paging keys mean the same thing here. */
        [[nodiscard]] std::vector<std::string> journalLines(
            const model::BattleJournal& journal,
            const common::Theme& theme,
            const int skipped
        ) {
            const std::deque<flow::MatchEvent>& entries = journal.entries();
            const auto total = static_cast<int>(entries.size());
            const int from = std::clamp(skipped, 0, common::state::furthestLogScroll(total));

            std::vector<std::string> lines{
                from > 0 ? "LOG  (" + std::to_string(from) + " back)" : "LOG"
            };

            for (int step = 0; step < common::state::LOG_VISIBLE_LINES; ++step) {
                const int index = total - 1 - from - step;
                if (index < 0) {
                    break;
                }

                lines.push_back(
                    "  " +
                    common::render::narrate(entries[static_cast<std::size_t>(index)], theme).text
                );
            }

            return lines;
        }

        [[nodiscard]] std::string standingOf(const flow::Match& match) {
            if (match.phase() == flow::MatchPhase::Finished) {
                return "YOUR FLEET IS GONE";
            }

            return match.isPlayerTurn() ? "your turn" : "the enemy's turn";
        }

        [[nodiscard]] std::string battleTitle(const flow::Match& match) {
            return "BATTLE   round " + std::to_string(match.roundNumber()) + "   " +
                   standingOf(match);
        }

        [[nodiscard]] std::vector<PlainKey> battleKeys(const bool isFinished) {
            if (isFinished) {
                return {{"esc", "back to the menu"}};
            }

            return {
                {"arrows", "take aim"},
                {"enter", "fire"},
                {"k", "use the next skill"},
                {"pgup", "further back through the log"},
                {"esc", "back to the menu"},
            };
        }
    }  // namespace

    PlainBattleView::PlainBattleView(const common::PresentationContext& context) noexcept
        : context_(context) {}

    common::render::Frame PlainBattleView::render(int, int) {
        const flow::Match& match = context_.game().match();
        const common::state::BattleState& state = context_.state().battle;
        const bool isFinished = match.phase() == flow::MatchPhase::Finished;

        return PlainPage{}
            .line(battleTitle(match))
            .blank()
            .line("YOUR WATERS")
            .lines(plainBoardLines(match.playerBoard(), core::Visibility::Owner, {}))
            .blank()
            .line("ENEMY WATERS")
            .lines(plainBoardLines(
                match.computerBoard(),
                core::Visibility::Opponent,
                PlainBoardOverlay{.cursor = state.target}
            ))
            .blank()
            .line(plainBoardLegend())
            .blank()
            .line("  aiming at " + common::render::coordinateLabel(state.target))
            .blank()
            .line(skillBankLine(match))
            .blank()
            .lines(journalLines(context_.game().journal(), context_.theme(), state.logScroll))
            .blank()
            .keys(battleKeys(isFinished))
            .blank()
            .notices(context_.application())
            .frame();
    }
}  // namespace cpp_warships::head::plain
