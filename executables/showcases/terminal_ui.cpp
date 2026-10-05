#include <array>
#include <cstddef>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/table.hpp>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace {
    enum class CellState {
        Water,
        Ship,
        Hit,
        Miss,
    };

    using Field = std::vector<std::vector<CellState>>;

    constexpr std::size_t FIELD_SIZE = 10;

    /** @brief Which cell of a field the player has walked onto. */
    struct Selection {
        std::size_t row = 0;
        std::size_t column = 0;
    };

    std::string glyphOf(const CellState state) {
        static const std::map<CellState, std::string> GLYPHS = {
            {CellState::Water, " "},
            {CellState::Ship, "#"},
            {CellState::Hit, "X"},
            {CellState::Miss, "o"},
        };

        return GLYPHS.at(state);
    }

    ftxui::Element cellElement(const CellState state, const bool isSelected) {
        ftxui::Element cell = ftxui::text(glyphOf(state)) | ftxui::bold | ftxui::hcenter |
                              ftxui::xflex | ftxui::size(ftxui::HEIGHT, ftxui::EQUAL, 1);

        return isSelected ? std::move(cell) | ftxui::bgcolor(ftxui::Color::GrayDark) : cell;
    }

    ftxui::Element fieldTable(const Field& field, const Selection& selection) {
        std::vector<ftxui::Elements> rows(field.size());
        for (std::size_t row = 0; row < field.size(); ++row) {
            for (std::size_t column = 0; column < field[row].size(); ++column) {
                const bool isSelected = row == selection.row && column == selection.column;
                rows[row].push_back(cellElement(field[row][column], isSelected));
            }
        }

        ftxui::Table table{rows};
        table.SelectAll().Border(ftxui::DOUBLE);
        table.SelectAll().SeparatorVertical(ftxui::LIGHT);
        table.SelectAll().SeparatorHorizontal(ftxui::LIGHT);

        return table.Render() | ftxui::xflex;
    }

    /** @brief Moves @p selection one cell the way an arrow key points, when it stays on a field
     * @p size cells across; whether the event was such an arrow. */
    bool moveSelection(Selection& selection, const ftxui::Event& event, const std::size_t size) {
        static const std::map<ftxui::Event, std::pair<int, int>> STEPS = {
            {ftxui::Event::ArrowUp, {-1, 0}},
            {ftxui::Event::ArrowDown, {1, 0}},
            {ftxui::Event::ArrowLeft, {0, -1}},
            {ftxui::Event::ArrowRight, {0, 1}},
        };
        const auto stepped = [size](const std::size_t from, const int by) {
            const long long target = static_cast<long long>(from) + by;
            const bool isOnField = target >= 0 && target < static_cast<long long>(size);
            return isOnField ? static_cast<std::size_t>(target) : from;
        };

        const auto step = STEPS.find(event);
        if (step == STEPS.end()) {
            return false;
        }

        selection.row = stepped(selection.row, step->second.first);
        selection.column = stepped(selection.column, step->second.second);
        return true;
    }

    ftxui::Component fieldComponent(const Field& field) {
        auto selection = std::make_shared<Selection>();
        const Field* shown = &field;

        const auto draw = [selection, shown] { return fieldTable(*shown, *selection); };
        const auto steer = [selection, shown](const ftxui::Event& event) {
            return moveSelection(*selection, event, shown->size());
        };

        return ftxui::CatchEvent(ftxui::Renderer(draw), steer);
    }

    Field fieldWith(
        const std::vector<std::pair<std::pair<std::size_t, std::size_t>, CellState>>& cells
    ) {
        Field field(FIELD_SIZE, std::vector<CellState>(FIELD_SIZE, CellState::Water));
        for (const auto& [position, state] : cells) {
            field[position.first][position.second] = state;
        }

        return field;
    }

    bool isQuit(const ftxui::Event& event) {
        return event == ftxui::Event::Escape || (event.is_character() && event.character() == "q");
    }
}  // namespace

int main() {
    const Field ownField = fieldWith({
        {{2, 3}, CellState::Ship},
        {{2, 4}, CellState::Ship},
        {{2, 5}, CellState::Hit},
        {{5, 7}, CellState::Hit},
        {{6, 1}, CellState::Miss},
        {{8, 8}, CellState::Miss},
    });
    const Field enemyField = fieldWith({
        {{4, 3}, CellState::Ship},
        {{5, 3}, CellState::Ship},
        {{6, 3}, CellState::Ship},
        {{1, 2}, CellState::Hit},
        {{7, 8}, CellState::Hit},
        {{8, 8}, CellState::Hit},
        {{9, 0}, CellState::Miss},
        {{4, 4}, CellState::Miss},
    });

    auto screen = ftxui::ScreenInteractive::TerminalOutput();
    const std::array<ftxui::Component, 2> fields{
        fieldComponent(ownField),
        fieldComponent(enemyField)
    };
    std::size_t activeField = 0;

    const auto draw = [&fields] {
        return ftxui::hbox({fields[0]->Render(), ftxui::separator(), fields[1]->Render()});
    };
    const auto route = [&](const ftxui::Event& event) {
        if (event == ftxui::Event::Tab) {
            activeField = 1 - activeField;
            return true;
        }
        if (isQuit(event)) {
            screen.Exit();
            return true;
        }

        return fields[activeField]->OnEvent(event);
    };

    screen.Loop(ftxui::CatchEvent(ftxui::Renderer(draw), route));
    return 0;
}
