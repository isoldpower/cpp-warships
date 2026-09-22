#include <game_tui/CellAppearance.h>

#include <functional>
#include <unordered_map>

namespace cpp_warships::game_tui {
    namespace {
        using AppearanceFactory = std::function<CellAppearance(const Theme&)>;

        const std::unordered_map<game_core::CellState, AppearanceFactory> APPEARANCE_BY_STATE{
                {game_core::CellState::Water,
                 [](const Theme& theme) {
                     return CellAppearance{"·", theme.water};
                 }},
                {game_core::CellState::Ship,
                 [](const Theme& theme) {
                     return CellAppearance{"█", theme.ship};
                 }},
                {game_core::CellState::Hit,
                 [](const Theme& theme) {
                     return CellAppearance{"✳", theme.hit};
                 }},
                {game_core::CellState::Sunk,
                 [](const Theme& theme) {
                     return CellAppearance{"✖", theme.sunk};
                 }},
                {game_core::CellState::Miss, [](const Theme& theme) {
                     return CellAppearance{"◌", theme.miss};
                 }}
        };
    } // namespace

    CellAppearance appearanceOf(game_core::CellState state, const Theme& theme) {
        return APPEARANCE_BY_STATE.at(state)(theme);
    }
} // namespace cpp_warships::game_tui
