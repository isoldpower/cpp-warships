#pragma once

#include <application/head/common/Queries.h>
#include <application/head/common/host/Shell.h>

#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

namespace cpp_warships::head::tui {
    /** @brief Hosts the interface in the terminal: the one place that runs an FTXUI loop. */
    class TuiShell final : public common::host::Shell {
    public:
        explicit TuiShell(common::ThemeQuery theme);

        void run(
            common::PresentationContext& context,
            common::render::RendererSet& renderers,
            common::input::EventPipeline& pipeline,
            const common::host::SessionFinishedQuery& isFinished
        ) override;

    private:
        /** @brief Draws whichever screen @p context says is showing, filling the terminal. */
        [[nodiscard]] static ftxui::Element frameOfActiveScreen(
            const common::PresentationContext& context,
            common::render::RendererSet& renderers
        );

        /** @brief Hands @p event to the game, unless it is the terminal answering about key
         * codes or asking to stop; whether anything took it. */
        bool routeEvent(
            const ftxui::Event& event,
            common::PresentationContext& context,
            common::input::EventPipeline& pipeline,
            const common::host::SessionFinishedQuery& isFinished
        );

        ftxui::ScreenInteractive interactiveScreen_;
        common::ThemeQuery theme_;
    };
}  // namespace cpp_warships::head::tui
