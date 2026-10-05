#include <application/head/common/render/Frame.h>

#include <algorithm>
#include <cstddef>

namespace cpp_warships::head::common::render {
    namespace {
        constexpr const char* STYLE_RESET = "\033[0m";

        [[nodiscard]] std::string foregroundCode(const Color color) {
            return "\033[38;2;" + std::to_string(color.red) + ";" + std::to_string(color.green) +
                   ";" + std::to_string(color.blue) + "m";
        }

        [[nodiscard]] std::string backgroundCode(const Color color) {
            return "\033[48;2;" + std::to_string(color.red) + ";" + std::to_string(color.green) +
                   ";" + std::to_string(color.blue) + "m";
        }

        /** @brief Whether a cell asks for anything the terminal would not do by itself. */
        [[nodiscard]] bool isStyled(const FrameCell& cell) {
            return cell.fill.has_value() || cell.ink.has_value() || cell.isBold;
        }

        /** @brief The escape codes that switch on @p cell's weight and colours. */
        [[nodiscard]] std::string styleOf(const FrameCell& cell) {
            std::string style;
            style += cell.isBold ? "\033[1m" : "";
            style += cell.ink.has_value() ? foregroundCode(*cell.ink) : "";
            style += cell.fill.has_value() ? backgroundCode(*cell.fill) : "";
            return style;
        }
    }  // namespace

    Frame::Frame(const int width, const int height)
        : width_(std::max(0, width))
        , height_(std::max(0, height))
        , cells_(static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_)) {}

    int Frame::width() const noexcept {
        return width_;
    }

    int Frame::height() const noexcept {
        return height_;
    }

    const FrameCell& Frame::at(const int column, const int row) const {
        return cells_
            [static_cast<std::size_t>(row) * static_cast<std::size_t>(width_) +
             static_cast<std::size_t>(column)];
    }

    FrameCell& Frame::at(const int column, const int row) {
        return cells_
            [static_cast<std::size_t>(row) * static_cast<std::size_t>(width_) +
             static_cast<std::size_t>(column)];
    }

    void Frame::write(const int column, const int row, const std::string& text) {
        if (row < 0 || row >= height_) {
            return;
        }

        for (std::size_t step = 0; step < text.size(); ++step) {
            const int target = column + static_cast<int>(step);
            if (target < 0 || target >= width_) {
                continue;
            }

            at(target, row).glyph = std::string(1, text[step]);
        }
    }

    Frame frameOfLines(const std::vector<std::string>& lines) {
        const auto longest = std::max_element(
            lines.begin(),
            lines.end(),
            [](const std::string& left, const std::string& right) {
                return left.size() < right.size();
            }
        );

        const int width = longest == lines.end() ? 0 : static_cast<int>(longest->size());

        Frame frame{width, static_cast<int>(lines.size())};
        for (std::size_t row = 0; row < lines.size(); ++row) {
            frame.write(0, static_cast<int>(row), lines[row]);
        }

        return frame;
    }

    std::string frameToText(const Frame& frame) {
        std::string text;

        for (int row = 0; row < frame.height(); ++row) {
            bool isStyleOpen = false;

            for (int column = 0; column < frame.width(); ++column) {
                const FrameCell& cell = frame.at(column, row);

                const bool isStyledCell = isStyled(cell);
                text += isStyledCell ? styleOf(cell) : "";
                text += !isStyledCell && isStyleOpen ? STYLE_RESET : "";
                isStyleOpen = isStyledCell;

                text += cell.glyph;
            }

            text += isStyleOpen ? STYLE_RESET : "";

            while (!text.empty() && text.back() == ' ') {
                text.pop_back();
            }

            text += '\n';
        }

        return text;
    }
}  // namespace cpp_warships::head::common::render
