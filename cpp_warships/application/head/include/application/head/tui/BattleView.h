#pragma once

#include <application/core/Coordinate.h>
#include <application/flow/Match.h>
#include <application/head/common/Queries.h>
#include <application/head/common/Theme.h>
#include <application/head/common/state/BattleState.h>
#include <application/head/tui/BoardView.h>
#include <application/head/tui/FtxuiView.h>
#include <application/head/tui/KeyHint.h>
#include <application/head/tui/ScrollPanel.h>
#include <application/model/BattleJournal.h>

#include <ftxui/dom/elements.hpp>
#include <optional>

namespace cpp_warships::head::common {
    class PresentationContext;
}

namespace cpp_warships::head::tui {
    /** @brief Draws both fleets, the skills in the bank and the story so far.
     * Reads the match and returns elements; it changes nothing. */
    class BattleView final : public FtxuiRenderer {
    public:
        BattleView(
            const common::PresentationContext& context,
            common::input::GridGeometry& geometry
        ) noexcept;

    protected:
        [[nodiscard]] ftxui::Element renderElement() override;
        void publishLayout() override;

    private:
        const common::PresentationContext& context_;
        common::input::GridGeometry& geometry_;
        BoardView ownWatersView_;
        BoardView enemyWatersView_;
        ScrollPanel ownWatersPanel_;
        ScrollPanel enemyWatersPanel_;
        ScrollPanel skillsPanel_;
        ScrollPanel shortcutsPanel_;
        KeyHotspots hotspots_;
        ScrollPanel logPanel_;
    };
}  // namespace cpp_warships::head::tui
