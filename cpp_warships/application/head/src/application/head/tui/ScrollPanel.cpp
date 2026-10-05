#include <application/head/common/PresentationContext.h>
#include <application/head/common/input/PanelScrolling.h>
#include <application/head/tui/FtxuiPalette.h>
#include <application/head/tui/ScrollPanel.h>

#include <algorithm>
#include <ftxui/dom/node.hpp>
#include <ftxui/dom/requirement.hpp>
#include <ftxui/screen/screen.hpp>
#include <memory>
#include <utility>

namespace cpp_warships::head::tui {
    namespace {
        /** @brief Drawn beside a panel's heading while it holds the focus, and blanked to
         * the same width while it does not, so headings never shift. */
        constexpr const char* FOCUS_MARKER = "▸ ";
        constexpr const char* NO_FOCUS_MARKER = "  ";

        /** @brief Where a scroll bar's thumb sits along a track @p trackLength long, for a
         * window of that length scrolled @p offset into @p contentLength. */
        struct Thumb {
            int start = 0;
            int length = 0;
        };

        [[nodiscard]] Thumb thumbOf(
            const int trackLength,
            const int contentLength,
            const int offset
        ) {
            if (trackLength <= 0 || contentLength <= 0) {
                return {};
            }

            const int length =
                std::clamp(trackLength * trackLength / contentLength, 1, trackLength);
            const int furthestOffset = std::max(1, contentLength - trackLength);
            const int start = (trackLength - length) * offset / furthestOffset;

            return {.start = std::clamp(start, 0, trackLength - length), .length = length};
        }

        /** @brief Passes its one child through untouched, noting the box it was laid out in. */
        class WholeBoxNode final : public ftxui::Node {
        public:
            WholeBoxNode(ftxui::Element child, ftxui::Box& box)
                : ftxui::Node(ftxui::Elements{std::move(child)})
                , recordedBox_(box) {}

            void ComputeRequirement() override {
                children_[0]->ComputeRequirement();
                requirement_ = children_[0]->requirement();
            }

            void SetBox(ftxui::Box box) override {
                ftxui::Node::SetBox(box);
                recordedBox_ = box;
                children_[0]->SetBox(box);
            }

        private:
            ftxui::Box& recordedBox_;
        };

        /** @brief Asks for less height than its one child would, and passes on whatever it gets. */
        class ModestHeightNode final : public ftxui::Node {
        public:
            ModestHeightNode(ftxui::Element child, const int lines)
                : ftxui::Node(ftxui::Elements{std::move(child)})
                , lines_(lines) {}

            void ComputeRequirement() override {
                children_[0]->ComputeRequirement();
                requirement_ = children_[0]->requirement();
                requirement_.min_y = std::min(requirement_.min_y, lines_);
            }

            void SetBox(ftxui::Box box) override {
                ftxui::Node::SetBox(box);
                children_[0]->SetBox(box);
            }

        private:
            int lines_;
        };

        /** @brief Lays its one child out at full size behind a smaller window, slid along by
         * however far it is scrolled, and paints only what the window shows. */
        class ScrollableNode final : public ftxui::Node {
        public:
            ScrollableNode(
                ftxui::Element content,
                const common::state::ScrollOffset wanted,
                ScrollLayout& layout,
                ScrollBarColors colors
            )
                : ftxui::Node(ftxui::Elements{std::move(content)})
                , wanted_(wanted)
                , layout_(layout)
                , colors_(std::move(colors)) {}

            void ComputeRequirement() override {
                children_[0]->ComputeRequirement();
                const ftxui::Requirement content = children_[0]->requirement();

                contentWidth_ = content.min_x;
                contentHeight_ = content.min_y;

                requirement_ = ftxui::Requirement{};
                requirement_.min_x = content.min_x;
                requirement_.min_y = content.min_y;
                requirement_.flex_grow_x = content.flex_grow_x;
                requirement_.flex_grow_y = content.flex_grow_y;
                requirement_.flex_shrink_x = 1;
                requirement_.flex_shrink_y = 1;
            }

            void SetBox(ftxui::Box box) override {
                ftxui::Node::SetBox(box);

                const int width = std::max(0, box.x_max - box.x_min + 1);
                const int height = std::max(0, box.y_max - box.y_min + 1);

                isOverflowingDown_ = contentHeight_ > height;
                isOverflowingAcross_ = contentWidth_ > width - (isOverflowingDown_ ? 1 : 0);
                if (!isOverflowingDown_ && isOverflowingAcross_) {
                    isOverflowingDown_ = contentHeight_ > height - 1;
                }

                const int windowWidth = std::max(0, width - (isOverflowingDown_ ? 1 : 0));
                const int windowHeight = std::max(0, height - (isOverflowingAcross_ ? 1 : 0));
                const int offsetX =
                    std::clamp(wanted_.x, 0, std::max(0, contentWidth_ - windowWidth));
                const int offsetY =
                    std::clamp(wanted_.y, 0, std::max(0, contentHeight_ - windowHeight));

                window_ = ftxui::Box{
                    .x_min = box.x_min,
                    .x_max = box.x_min + windowWidth - 1,
                    .y_min = box.y_min,
                    .y_max = box.y_min + windowHeight - 1
                };

                const ftxui::Box contentBox{
                    .x_min = box.x_min - offsetX,
                    .x_max = box.x_min - offsetX + std::max(contentWidth_, windowWidth) - 1,
                    .y_min = box.y_min - offsetY,
                    .y_max = box.y_min - offsetY + std::max(contentHeight_, windowHeight) - 1
                };
                children_[0]->SetBox(contentBox);

                layout_ = ScrollLayout{
                    .isLaidOut = true,
                    .window = window_,
                    .contentWidth = contentWidth_,
                    .contentHeight = contentHeight_,
                    .offsetX = offsetX,
                    .offsetY = offsetY
                };
            }

            void Render(ftxui::Screen& screen) override {
                const ftxui::Box outer = screen.stencil;
                screen.stencil = ftxui::Box::Intersection(window_, outer);
                children_[0]->Render(screen);
                screen.stencil = outer;

                if (isOverflowingDown_) {
                    paintBarDown(screen);
                }
                if (isOverflowingAcross_) {
                    paintBarAcross(screen);
                }
            }

        private:
            void paintBarDown(ftxui::Screen& screen) const {
                const int column = window_.x_max + 1;
                const int length = window_.y_max - window_.y_min + 1;
                const Thumb thumb = thumbOf(length, contentHeight_, layout_.offsetY);

                for (int step = 0; step < length; ++step) {
                    const bool isThumb = step >= thumb.start && step < thumb.start + thumb.length;
                    paintBarCell(
                        screen,
                        column,
                        window_.y_min + step,
                        isThumb ? "┃" : "│",
                        isThumb
                    );
                }
            }

            void paintBarAcross(ftxui::Screen& screen) const {
                const int row = window_.y_max + 1;
                const int length = window_.x_max - window_.x_min + 1;
                const Thumb thumb = thumbOf(length, contentWidth_, layout_.offsetX);

                for (int step = 0; step < length; ++step) {
                    const bool isThumb = step >= thumb.start && step < thumb.start + thumb.length;
                    paintBarCell(screen, window_.x_min + step, row, isThumb ? "━" : "─", isThumb);
                }
            }

            void paintBarCell(
                ftxui::Screen& screen,
                const int column,
                const int row,
                const char* glyph,
                const bool isThumb
            ) const {
                if (!screen.stencil.Contain(column, row)) {
                    return;
                }

                ftxui::Cell& cell = screen.CellAt(column, row);
                cell.character = glyph;
                cell.foreground_color = isThumb ? colors_.thumb : colors_.track;
            }

            common::state::ScrollOffset wanted_;
            ScrollLayout& layout_;
            ScrollBarColors colors_;
            ftxui::Box window_;
            int contentWidth_ = 0;
            int contentHeight_ = 0;
            bool isOverflowingDown_ = false;
            bool isOverflowingAcross_ = false;
        };
    }  // namespace

    ftxui::Decorator reflectWholeBox(ftxui::Box& box) {
        return [&box](ftxui::Element child) -> ftxui::Element {
            return std::make_shared<WholeBoxNode>(std::move(child), box);
        };
    }

    ftxui::Decorator heightAskedAtMost(const int lines) {
        return [lines](ftxui::Element child) -> ftxui::Element {
            return std::make_shared<ModestHeightNode>(std::move(child), lines);
        };
    }

    ftxui::Element scrollable(
        ftxui::Element content,
        const common::state::ScrollOffset wanted,
        ScrollLayout& layout,
        ScrollBarColors colors
    ) {
        return std::make_shared<ScrollableNode>(
            std::move(content),
            wanted,
            layout,
            std::move(colors)
        );
    }

    ScrollPanel::ScrollPanel(const common::input::ScreenRegion region) noexcept
        : region_(region) {}

    ftxui::Element ScrollPanel::render(
        const common::PresentationContext& context,
        const common::ScreenKind screen,
        ftxui::Element heading,
        ftxui::Element content
    ) {
        const common::Theme& theme = context.theme();
        const bool isFocused = common::input::focusedPanel(context.state(), screen) == region_;
        const ScrollBarColors colors{
            .track = toFtxuiColor(theme.border),
            .thumb = toFtxuiColor(theme.accent)
        };

        return ftxui::vbox(
                   {ftxui::hbox(
                        {ftxui::text(isFocused ? FOCUS_MARKER : NO_FOCUS_MARKER) | ftxui::bold |
                             color(theme.accent),
                         std::move(heading) | ftxui::flex}
                    ),
                    scrollable(
                        std::move(content),
                        common::input::scrollOf(context.state(), region_),
                        layout_,
                        colors
                    ) | ftxui::flex_grow}
               ) |
               ftxui::flex_shrink;
    }

    void ScrollPanel::publish(common::input::GridGeometry& geometry) const {
        if (!layout_.isLaidOut) {
            return;
        }

        geometry.rememberPanel(
            region_,
            common::input::PanelExtent{
                .left = layout_.window.x_min,
                .top = layout_.window.y_min,
                .width = std::max(0, layout_.window.x_max - layout_.window.x_min + 1),
                .height = std::max(0, layout_.window.y_max - layout_.window.y_min + 1),
                .contentWidth = layout_.contentWidth,
                .contentHeight = layout_.contentHeight,
                .offsetX = layout_.offsetX,
                .offsetY = layout_.offsetY
            }
        );
    }
}  // namespace cpp_warships::head::tui
