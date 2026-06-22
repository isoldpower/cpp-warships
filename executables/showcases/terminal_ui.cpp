#include <cstddef>
#include <ftxui/component/screen_interactive.hpp>
#include "ftxui/component/component.hpp"
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <map>
#include <vector>


using FieldMock = std::vector<std::vector<std::size_t>>;

enum class CellState : std::size_t {
    Water = 0,
    Ship  = 1,
    Hit   = 2,
    Miss  = 3,
};

auto cell_glyph(const CellState state) -> std::string {
    switch (state) {
        case CellState::Ship:  return "#";
        case CellState::Hit:   return "X";
        case CellState::Miss:  return "o";
        case CellState::Water:
        default:               return " ";
    }
}

auto draw_single_cell(const std::size_t value, const bool selected) -> ftxui::Element {
    const auto glyph = cell_glyph(static_cast<CellState>(value));

    auto cell = ftxui::text(glyph)
              | ftxui::bold
              | ftxui::hcenter
              | ftxui::xflex
              | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 1);

    if (selected) {
        cell = cell | ftxui::bgcolor(ftxui::Color::GrayDark);
    }

    return cell;
}

struct Selection {
    std::size_t row = 0;
    std::size_t col = 0;
};

auto draw_field(const FieldMock& field_state) -> ftxui::Component {
    const std::size_t rows = field_state.size();
    const std::size_t cols = rows == 0 ? 0 : field_state.front().size();

    auto sel = std::make_shared<Selection>();
    const FieldMock* field_state_copy = &field_state;

    auto renderer = ftxui::Renderer([sel, field_state_copy, rows] {
        std::vector<ftxui::Elements> elements(rows);
        for (std::size_t i = 0; i < rows; ++i) {
            for (std::size_t j = 0; j < (*field_state_copy)[i].size(); ++j) {
                const bool selected = (i == sel->row && j == sel->col);
                elements[i].push_back(draw_single_cell((*field_state_copy)[i][j], selected));
            }
        }

        auto table = ftxui::Table(elements);
        table.SelectAll().Border(ftxui::DOUBLE);
        table.SelectAll().SeparatorVertical(ftxui::LIGHT);
        table.SelectAll().SeparatorHorizontal(ftxui::LIGHT);

        return table.Render() | ftxui::xflex;
    });

    return ftxui::CatchEvent(renderer, [sel, rows, cols](const ftxui::Event& event) {
        if (event == ftxui::Event::ArrowUp && sel->row > 0) {
            --sel->row;
        } else if (event == ftxui::Event::ArrowDown && sel->row + 1 < rows) {
            ++sel->row;
        } else if (event == ftxui::Event::ArrowLeft && sel->col > 0) {
            --sel->col;
        } else if (event == ftxui::Event::ArrowRight && sel->col + 1 < cols) {
            ++sel->col;
        } else {
            return false;
        }
        return true;
    });
}

int main() {
    const std::size_t field_size = 10;
    std::vector<std::vector<std::size_t>> field = std::vector(
        field_size,
        std::vector<std::size_t>(field_size, 0));

    field[2][3] = static_cast<std::size_t>(CellState::Ship);
    field[2][4] = static_cast<std::size_t>(CellState::Ship);
    field[2][5] = static_cast<std::size_t>(CellState::Hit);
    field[5][7] = static_cast<std::size_t>(CellState::Hit);
    field[6][1] = static_cast<std::size_t>(CellState::Miss);
    field[8][8] = static_cast<std::size_t>(CellState::Miss);

    std::vector<std::vector<std::size_t>> field2 = std::vector(
        field_size,
        std::vector<std::size_t>(field_size, 0));

    field2[4][3] = static_cast<std::size_t>(CellState::Ship);
    field2[5][3] = static_cast<std::size_t>(CellState::Ship);
    field2[6][3] = static_cast<std::size_t>(CellState::Ship);
    field2[1][2] = static_cast<std::size_t>(CellState::Hit);
    field2[7][8] = static_cast<std::size_t>(CellState::Hit);
    field2[8][8] = static_cast<std::size_t>(CellState::Hit);
    field2[9][0] = static_cast<std::size_t>(CellState::Miss);
    field2[4][4] = static_cast<std::size_t>(CellState::Miss);

    auto screen = ftxui::ScreenInteractive::TerminalOutput();

    auto field1_component = draw_field(field);
    auto field2_component = draw_field(field2);

    auto active_board = std::make_shared<int>(0);

    auto renderer = ftxui::Renderer([&] {
        return ftxui::hbox({
            field1_component->Render(),
            ftxui::separator(),
            field2_component->Render(),
        });
    });

    const auto root = ftxui::CatchEvent(renderer, [&](const ftxui::Event& event) {
        if (event == ftxui::Event::Tab) {
            *active_board = 1 - *active_board;
            return true;
        } else if (event == ftxui::Event::Escape ||
            (event.is_character() && event.character() == "q")) {
            screen.Exit();
            return true;
        }

        const auto& target = *active_board == 0 ? field1_component : field2_component;
        return target->OnEvent(event);
    });

    screen.Loop(root);

    return 0;
}