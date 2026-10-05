#include <application/head/common/PresentationContext.h>
#include <application/head/common/render/EventNarration.h>
#include <application/head/tui/BattleView.h>
#include <application/head/tui/FtxuiNotices.h>
#include <application/head/tui/FtxuiPalette.h>
#include <application/head/tui/KeyHint.h>
#include <application/head/tui/ScreenParts.h>
#include <application/head/tui/ScrollPanel.h>

#include <algorithm>
#include <cstddef>
#include <deque>
#include <functional>
#include <map>
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

        /** @brief The most height the log asks for, its heading and a few entries, however long
         * the story gets; it still grows into whatever room the other panels leave. */
        constexpr int LOG_LINES_ASKED = 4;
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

        /** @brief One skill of the bank: its place in the queue and its name, the next one marked.
         */
        ftxui::Element skillOrderRow(
            const common::Theme& theme,
            const flow::Match& match,
            const std::size_t position,
            const flow::SkillKind kind
        ) {
            const bool isNext = position == 0;
            return ftxui::hbox(
                {ftxui::text(std::to_string(position + 1) + "  ") | color(theme.textMuted),
                 ftxui::text(common::render::skillName(kind)) |
                     color(isNext ? theme.text : theme.textMuted),
                 ftxui::filler(),
                 isNext ? nextMarker(theme, match) : ftxui::text("")}
            );
        }

        /** @brief The bank in the order it will be spent, oldest first, the next one marked. */
        ftxui::Element skillOrder(const common::Theme& theme, const flow::Match& match) {
            const std::deque<flow::SkillKind>& banked = match.skills().pending();
            const auto fits = static_cast<std::size_t>(SKILL_ORDER_LINES);
            const std::size_t listed = banked.size() > fits ? fits - 1 : banked.size();

            std::vector<ftxui::Element> rows;
            for (std::size_t position = 0; position < listed; ++position) {
                rows.push_back(skillOrderRow(theme, match, position, banked[position]));
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
                                      : std::move(panels.story) |
                                            heightAskedAtMost(LOG_LINES_ASKED) | ftxui::yflex_grow,
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
                      std::move(panels.story) | heightAskedAtMost(LOG_LINES_ASKED) |
                          ftxui::yflex_grow,
                      divider(theme),
                      std::move(panels.shortcuts)}
                 ) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, sidePanelWidth)}
            );
        }

        /** @brief The screen every panel of this view belongs to. */
        constexpr common::ScreenKind SCREEN = common::ScreenKind::Battle;

        /** @brief How the log is shown on a screen of @p tier: folding only where it is narrow. */
        [[nodiscard]] LogFolding foldingOf(const LayoutTier tier, const bool isCollapsed) {
            if (tier != LayoutTier::Narrow) {
                return LogFolding::Fixed;
            }

            return isCollapsed ? LogFolding::Folded : LogFolding::Open;
        }

        /** @brief How the panels of a battle are arranged on a screen of @p tier. */
        using Arrangement = std::function<ftxui::Element(const common::Theme&, BattlePanels, int)>;

        [[nodiscard]] const Arrangement& arrangementFor(const LayoutTier tier) {
            static const std::map<LayoutTier, Arrangement> ARRANGEMENTS = {
                {LayoutTier::Narrow,
                 [](const common::Theme& theme, BattlePanels panels, int) {
                     return stackedBody(theme, std::move(panels));
                 }},
                {LayoutTier::Laptop,
                 [](const common::Theme& theme, BattlePanels panels, int) {
                     return stripBelowBody(theme, std::move(panels));
                 }},
                {LayoutTier::Wide,
                 [](const common::Theme& theme, BattlePanels panels, const int screenWidth) {
                     return sideBarBody(theme, std::move(panels), screenWidth);
                 }},
            };

            return ARRANGEMENTS.at(tier);
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
        const LogFolding folding = foldingOf(layoutTier(), context_.state().battle.isLogCollapsed);

        BattlePanels panels{
            .ownWaters = renderOwnWaters(),
            .enemyWaters = renderEnemyWaters(),
            .skills = renderSkills(),
            .story = renderStory(),
            .shortcuts = renderShortcuts(),
            .isStoryFolded = folding == LogFolding::Folded
        };

        return screenFrame(
            theme,
            header(theme, context_.game().match(), isNarrow()),
            arrangementFor(layoutTier())(theme, std::move(panels), availableWidth()),
            noticeBlock(theme, context_.application())
        );
    }

    ftxui::Element BattleView::renderOwnWaters() {
        const common::Theme& theme = context_.theme();
        const ftxui::Element board = ownWatersView_.render(
            context_.game().match().playerBoard(),
            core::Visibility::Owner,
            theme,
            BoardOverlay{}
        );

        return ownWatersPanel_
            .render(context_, SCREEN, boardHeading(theme, "YOUR WATERS"), board | ftxui::center);
    }

    ftxui::Element BattleView::renderEnemyWaters() {
        const common::Theme& theme = context_.theme();
        const ftxui::Element board = enemyWatersView_.render(
            context_.game().match().computerBoard(),
            core::Visibility::Opponent,
            theme,
            BoardOverlay{.cursor = context_.state().battle.target}
        );

        return enemyWatersPanel_
            .render(context_, SCREEN, boardHeading(theme, "ENEMY WATERS"), board | ftxui::center);
    }

    ftxui::Element BattleView::renderSkills() {
        const common::Theme& theme = context_.theme();
        return skillsPanel_.render(
            context_,
            SCREEN,
            sectionHeading(theme, "SKILLS"),
            skillBank(theme, context_.game().match())
        );
    }

    ftxui::Element BattleView::renderShortcuts() {
        const common::Theme& theme = context_.theme();
        const ftxui::Element legendElement = keyLegend(
            theme,
            legend(context_.game().match(), isNarrow()),
            hotspots_,
            !context_.state().isKeyboardLayoutFree
        );

        return shortcutsPanel_
            .render(context_, SCREEN, sectionHeading(theme, "KEYS"), legendElement);
    }

    ftxui::Element BattleView::renderStory() {
        const common::Theme& theme = context_.theme();
        const model::BattleJournal& journal = context_.game().journal();
        const common::state::BattleState& state = context_.state().battle;
        const LogFolding folding = foldingOf(layoutTier(), state.isLogCollapsed);

        return logPanel_.render(
            context_,
            SCREEN,
            journalHeading(theme, journal, state.logScroll, folding),
            folding == LogFolding::Folded ? ftxui::emptyElement() : journalLines(theme, journal)
        );
    }
}  // namespace cpp_warships::head::tui
