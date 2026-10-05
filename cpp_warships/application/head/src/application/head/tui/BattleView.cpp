#include <application/head/common/PresentationContext.h>
#include <application/head/common/render/EventNarration.h>
#include <application/head/tui/BattleView.h>
#include <application/head/tui/FtxuiNotices.h>
#include <application/head/tui/FtxuiPalette.h>
#include <application/head/tui/KeyHint.h>
#include <application/head/tui/ScrollPanel.h>

#include <algorithm>
#include <cstddef>
#include <deque>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace cpp_warships::head::tui {
    namespace {
        /** @brief The widest the side panel stands, and the most of the screen it may take,
         * in percent, so the boards keep the larger share. */
        constexpr int SIDE_PANEL_WIDTH = 46;
        constexpr int SIDE_PANEL_SHARE_PERCENT = 30;
        constexpr int WHOLE_PERCENT = 100;

        /** @brief The tallest the strip under the boards stands: a heading over the longest
         * list of keys the battle shows. */
        constexpr int BOTTOM_STRIP_HEIGHT = 7;
        constexpr int SKILL_ORDER_LINES = 4;

        ftxui::Element standing(const common::Theme& theme, const flow::Match& match) {
            if (match.phase() == flow::MatchPhase::Finished) {
                return ftxui::text("YOUR FLEET IS GONE") | ftxui::bold | color(theme.danger);
            }

            if (match.isPlayerTurn()) {
                return ftxui::text("your turn") | ftxui::bold | color(theme.success);
            }

            return ftxui::text("the enemy fires") | ftxui::bold | color(theme.danger);
        }

        ftxui::Element divider(const common::Theme& theme) {
            return ftxui::separator() | color(theme.border);
        }

        ftxui::Element roundAndStanding(const common::Theme& theme, const flow::Match& match) {
            std::vector<ftxui::Element> parts{
                ftxui::text("round " + std::to_string(match.roundNumber())) |
                    color(theme.textMuted),
                ftxui::text("   "),
                standing(theme, match)
            };

            if (match.isDoubleDamageArmed()) {
                parts.push_back(ftxui::text("   "));
                parts.push_back(ftxui::text("double damage armed") | color(theme.accent));
            }

            return ftxui::hbox(std::move(parts));
        }

        /** @brief The title and how the battle stands: on one line, or on two when the
         * screen is too narrow to hold them side by side. */
        ftxui::Element header(
            const common::Theme& theme,
            const flow::Match& match,
            const bool isNarrow
        ) {
            ftxui::Element title = ftxui::text("BATTLE") | ftxui::bold | color(theme.accent);
            if (isNarrow) {
                return ftxui::vbox({std::move(title), roundAndStanding(theme, match)});
            }

            return ftxui::hbox({std::move(title), ftxui::filler(), roundAndStanding(theme, match)});
        }

        ftxui::Element boardHeading(const common::Theme& theme, const std::string& title) {
            return ftxui::text(title) | ftxui::bold | color(theme.textMuted);
        }

        ftxui::Element sectionHeading(const common::Theme& theme, const std::string& title) {
            return ftxui::text(title) | ftxui::bold | color(theme.accent);
        }

        /** @brief How many of each skill are banked, in a settled order rather
         * than queue order. */
        std::vector<std::pair<flow::SkillKind, int>> bankedByKind(const flow::Match& match) {
            const std::deque<flow::SkillKind>& banked = match.skills().pending();

            std::vector<std::pair<flow::SkillKind, int>> tally;
            for (const flow::SkillKind kind : flow::ALL_SKILL_KINDS) {
                const auto held = static_cast<int>(std::count(banked.begin(), banked.end(), kind));
                if (held > 0) {
                    tally.emplace_back(kind, held);
                }
            }

            return tally;
        }

        /** @brief What the next skill will do when spent, said where the queue
         * can show it. */
        ftxui::Element nextMarker(const common::Theme& theme, const flow::Match& match) {
            const std::string label = match.nextSkillNeedsTarget() ? "next, where you aim" : "next";
            return ftxui::text(label) | color(theme.accent);
        }

        /** @brief One row saying how many of each kind are held, whatever order
         * they sit in. */
        ftxui::Element skillCounts(const common::Theme& theme, const flow::Match& match) {
            if (match.skills().isEmpty()) {
                return ftxui::text("none banked") | color(theme.textMuted);
            }

            std::vector<ftxui::Element> chips;
            for (const auto& [kind, held] : bankedByKind(match)) {
                if (!chips.empty()) {
                    chips.push_back(ftxui::text("  "));
                }

                chips.push_back(
                    ftxui::text(common::render::skillName(kind)) | color(theme.textMuted)
                );
                chips.push_back(
                    ftxui::text(" " + std::to_string(held)) | ftxui::bold | color(theme.accent)
                );
            }

            return ftxui::hbox(std::move(chips));
        }

        /** @brief The bank in the order it will be spent, oldest first, the next one marked. */
        ftxui::Element skillOrder(const common::Theme& theme, const flow::Match& match) {
            const std::deque<flow::SkillKind>& banked = match.skills().pending();
            const auto fits = static_cast<std::size_t>(SKILL_ORDER_LINES);
            const std::size_t listed = banked.size() > fits ? fits - 1 : banked.size();

            std::vector<ftxui::Element> rows;
            for (std::size_t position = 0; position < listed; ++position) {
                const bool isNext = position == 0;
                rows.push_back(
                    ftxui::hbox(
                        {ftxui::text(std::to_string(position + 1) + "  ") | color(theme.textMuted),
                         ftxui::text(common::render::skillName(banked[position])) |
                             color(isNext ? theme.text : theme.textMuted),
                         ftxui::filler(),
                         isNext ? nextMarker(theme, match) : ftxui::text("")}
                    )
                );
            }

            if (banked.size() > listed) {
                const std::string rest = std::to_string(banked.size() - listed);
                rows.push_back(ftxui::text("+" + rest + " more skills") | color(theme.textMuted));
            }

            while (rows.size() < fits) {
                rows.push_back(ftxui::text(""));
            }

            return ftxui::vbox(std::move(rows)) |
                   ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, SKILL_ORDER_LINES);
        }

        ftxui::Element skillBank(const common::Theme& theme, const flow::Match& match) {
            return ftxui::vbox({skillCounts(theme, match), skillOrder(theme, match)});
        }

        /** @brief The whole story so far, newest first, for the log's panel to scroll through. */
        ftxui::Element journalLines(
            const common::Theme& theme,
            const model::BattleJournal& journal
        ) {
            const std::deque<flow::MatchEvent>& entries = journal.entries();

            std::vector<ftxui::Element> lines;
            for (auto entry = entries.rbegin(); entry != entries.rend(); ++entry) {
                const common::render::EventLine line = common::render::narrate(*entry, theme);
                lines.push_back(ftxui::text(line.text) | color(line.color));
            }

            if (lines.empty()) {
                lines.push_back(ftxui::text("nothing has happened yet") | color(theme.textMuted));
            }

            return ftxui::vbox(std::move(lines));
        }

        /** @brief How the log is shown: open in a column of its own, open but able to fold,
         * or folded down to its heading. */
        enum class LogFolding {
            Fixed,
            Open,
            Folded,
        };

        /** @brief The newest line of the story, squeezed to whatever room is left beside it. */
        ftxui::Element newestLine(const common::Theme& theme, const model::BattleJournal& journal) {
            if (journal.entries().empty()) {
                return ftxui::filler();
            }

            const common::render::EventLine line =
                common::render::narrate(journal.entries().back(), theme);
            return ftxui::text("  " + line.text) | color(theme.textMuted) | ftxui::xflex_shrink |
                   ftxui::xflex_grow;
        }

        ftxui::Element journalHeading(
            const common::Theme& theme,
            const model::BattleJournal& journal,
            int skipped,
            const LogFolding folding
        ) {
            const int total = static_cast<int>(journal.entries().size());
            const int from = std::clamp(skipped, 0, std::max(0, total - 1));

            if (folding == LogFolding::Folded) {
                return ftxui::hbox(
                    {sectionHeading(theme, "LOG"),
                     newestLine(theme, journal),
                     ftxui::text("  l show") | color(theme.textMuted)}
                );
            }

            std::vector<ftxui::Element> parts{sectionHeading(theme, "LOG"), ftxui::filler()};

            if (folding == LogFolding::Open) {
                parts.push_back(ftxui::text("l hide  ") | color(theme.textMuted));
            }

            if (from > 0) {
                parts.push_back(
                    ftxui::text(std::to_string(from) + " back") | color(theme.textMuted)
                );
            }

            return ftxui::hbox(std::move(parts));
        }

        std::vector<KeyHint> legend(const flow::Match& match, const bool canFoldLog) {
            if (match.phase() == flow::MatchPhase::Finished) {
                return {KeyHint{.key = "esc", .description = "back to the menu"}};
            }

            std::vector<KeyHint> hints{
                KeyHint{.key = "arrows", .description = "take aim"},
                KeyHint{.key = "enter", .description = "fire"},
                KeyHint{.key = "esc", .description = "back to the menu"}
            };

            if (!match.skills().isEmpty()) {
                hints.insert(
                    hints.begin() + 2,
                    KeyHint{.key = "k", .description = "use the next skill"}
                );
            }

            if (canFoldLog) {
                hints.insert(
                    hints.end() - 1,
                    KeyHint{.key = "l", .description = "show or hide the log"}
                );
            }

            return hints;
        }
        /** @brief Every panel of the battle screen, drawn and waiting to be arranged. */
        struct BattlePanels {
            ftxui::Element ownWaters;
            ftxui::Element enemyWaters;
            ftxui::Element skills;
            ftxui::Element story;
            ftxui::Element shortcuts;
            bool isStoryFolded = false;
        };

        /** @brief A board's panel, growing into spare height and the first to give it up. */
        ftxui::Element boardInColumn(ftxui::Element board) {
            return std::move(board) | ftxui::yflex_grow |
                   ftxui::yflex_shrink_factor(BOARD_PANEL_SHRINK_WEIGHT);
        }

        /** @brief A phone-sized screen: every panel in one column, the enemy's waters on top. */
        ftxui::Element stackedBody(const common::Theme& theme, BattlePanels panels) {
            return ftxui::vbox(
                {boardInColumn(std::move(panels.enemyWaters)),
                 divider(theme),
                 boardInColumn(std::move(panels.ownWaters)),
                 divider(theme),
                 std::move(panels.skills),
                 divider(theme),
                 panels.isStoryFolded ? std::move(panels.story)
                                      : std::move(panels.story) | ftxui::yflex_grow,
                 divider(theme),
                 std::move(panels.shortcuts)}
            );
        }

        /** @brief A laptop-sized screen: the boards side by side, the skills, log and keys
         * in a strip under them that a long log can never make taller. */
        ftxui::Element stripBelowBody(const common::Theme& theme, BattlePanels panels) {
            return ftxui::vbox(
                {boardInColumn(
                     ftxui::hbox(
                         {std::move(panels.ownWaters) | ftxui::flex,
                          divider(theme),
                          std::move(panels.enemyWaters) | ftxui::flex}
                     )
                 ),
                 divider(theme),
                 ftxui::hbox(
                     {std::move(panels.skills),
                      divider(theme),
                      std::move(panels.story) | ftxui::xflex,
                      divider(theme),
                      std::move(panels.shortcuts)}
                 ) | ftxui::size(ftxui::HEIGHT, ftxui::LESS_THAN, BOTTOM_STRIP_HEIGHT) |
                     ftxui::yflex_shrink}
            );
        }

        /** @brief A wide screen: the boards side by side, everything else in a side bar
         * that never takes more than its share of @p screenWidth. */
        ftxui::Element sideBarBody(
            const common::Theme& theme,
            BattlePanels panels,
            const int screenWidth
        ) {
            const int sidePanelWidth =
                std::min(SIDE_PANEL_WIDTH, screenWidth * SIDE_PANEL_SHARE_PERCENT / WHOLE_PERCENT);

            return ftxui::hbox(
                {std::move(panels.ownWaters) | ftxui::flex,
                 divider(theme),
                 std::move(panels.enemyWaters) | ftxui::flex,
                 divider(theme),
                 ftxui::vbox(
                     {std::move(panels.skills),
                      divider(theme),
                      std::move(panels.story) | ftxui::yflex_grow,
                      divider(theme),
                      std::move(panels.shortcuts)}
                 ) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, sidePanelWidth)}
            );
        }
    }  // namespace

    BattleView::BattleView(
        const common::PresentationContext& context,
        common::input::GridGeometry& geometry
    ) noexcept
        : context_(context)
        , geometry_(geometry)
        , ownWatersView_(geometry, common::input::ScreenRegion::OwnWaters)
        , enemyWatersView_(geometry, common::input::ScreenRegion::EnemyWaters)
        , ownWatersPanel_(common::input::ScreenRegion::OwnWaters)
        , enemyWatersPanel_(common::input::ScreenRegion::EnemyWaters)
        , skillsPanel_(common::input::ScreenRegion::Skills)
        , shortcutsPanel_(common::input::ScreenRegion::Shortcuts)
        , logPanel_(common::input::ScreenRegion::Log) {}

    void BattleView::publishLayout() {
        geometry_.rememberFoldable(common::input::ScreenRegion::Log, isNarrow());
        ownWatersView_.publishGeometry();
        enemyWatersView_.publishGeometry();
        for (
            const ScrollPanel* panel :
            {&ownWatersPanel_, &enemyWatersPanel_, &skillsPanel_, &shortcutsPanel_, &logPanel_}
        ) {
            panel->publish(geometry_);
        }
        hotspots_.publish(geometry_);
    }

    ftxui::Element BattleView::renderElement() {
        const common::Theme& theme = context_.theme();
        const flow::Match& match = context_.game().match();
        const model::BattleJournal& journal = context_.game().journal();
        const common::state::BattleState& state = context_.state().battle;
        constexpr common::ScreenKind SCREEN = common::ScreenKind::Battle;

        BoardOverlay ownOverlay;
        BoardOverlay enemyOverlay;
        enemyOverlay.cursor = state.target;

        ftxui::Element ownWaters = ownWatersPanel_.render(
            context_,
            SCREEN,
            boardHeading(theme, "YOUR WATERS"),
            ownWatersView_.render(match.playerBoard(), core::Visibility::Owner, theme, ownOverlay) |
                ftxui::center
        );
        ftxui::Element enemyWaters = enemyWatersPanel_.render(
            context_,
            SCREEN,
            boardHeading(theme, "ENEMY WATERS"),
            enemyWatersView_.render(
                match.computerBoard(),
                core::Visibility::Opponent,
                theme,
                enemyOverlay
            ) | ftxui::center
        );
        ftxui::Element skills =
            skillsPanel_
                .render(context_, SCREEN, sectionHeading(theme, "SKILLS"), skillBank(theme, match));
        ftxui::Element shortcuts = shortcutsPanel_.render(
            context_,
            SCREEN,
            sectionHeading(theme, "KEYS"),
            keyLegend(theme, legend(match, isNarrow()), hotspots_)
        );
        const LogFolding folding = !isNarrow()            ? LogFolding::Fixed
                                   : state.isLogCollapsed ? LogFolding::Folded
                                                          : LogFolding::Open;
        ftxui::Element story = logPanel_.render(
            context_,
            SCREEN,
            journalHeading(theme, journal, state.logScroll, folding),
            folding == LogFolding::Folded ? ftxui::emptyElement() : journalLines(theme, journal)
        );

        BattlePanels panels{
            .ownWaters = std::move(ownWaters),
            .enemyWaters = std::move(enemyWaters),
            .skills = std::move(skills),
            .story = std::move(story),
            .shortcuts = std::move(shortcuts),
            .isStoryFolded = folding == LogFolding::Folded
        };
        ftxui::Element body = isNarrow() ? stackedBody(theme, std::move(panels))
                              : isLaptop()
                                  ? stripBelowBody(theme, std::move(panels))
                                  : sideBarBody(theme, std::move(panels), availableWidth());

        return ftxui::vbox(
                   {header(theme, match, isNarrow()),
                    divider(theme),
                    std::move(body) | ftxui::flex,
                    noticeBlock(theme, context_.application())}
               ) |
               ftxui::border | color(theme.border) | bgcolor(theme.background) | ftxui::flex;
    }
}  // namespace cpp_warships::head::tui
