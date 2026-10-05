#include <application/head/common/PresentationContext.h>
#include <application/head/common/input/EventPipeline.h>
#include <application/head/common/input/KeyCodes.h>
#include <application/head/common/render/RendererSet.h>
#include <application/head/tui/FtxuiPalette.h>
#include <application/head/tui/FtxuiView.h>
#include <application/head/tui/TuiShell.h>
#include <platform/OSBrancher.h>

#include <ftxui/component/component.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/terminal.hpp>
#include <iostream>
#include <string_view>
#include <utility>

namespace cpp_warships::head::tui {
    namespace {
        /** @brief Blanks the alternate screen before the library switches into
         * it. */
        void blankAlternateScreen() {
            if (!platform::isOutputTerminal()) {
                return;
            }

            std::cout << "\033[?1049h\033[2J\033[H" << std::flush;
        }

        /** @brief Writes @p sequence to the terminal, when there is one to hear it. */
        void tellTerminal(const std::string_view sequence) {
            if (!platform::isOutputTerminal()) {
                return;
            }

            std::cout << sequence << std::flush;
        }

        /** @brief Asks the terminal to report keys as codes, and whether it can. */
        void askForKeyCodes() {
            tellTerminal(common::input::KEY_CODES_REQUEST);
            tellTerminal(common::input::KEY_CODES_QUERY);
        }

        /** @brief Notes in @p context when @p event shows the terminal reports keys by position;
         * whether it was only the terminal's answer, which the game has no use for. */
        bool isAboutKeyCodes(const ftxui::Event& event, common::PresentationContext& context) {
            const bool isAnswer = common::input::isKeyCodesAnswer(event.input());
            if (isAnswer || common::input::keystrokeOfKeyCode(event.input()).has_value()) {
                context.state().isKeyboardLayoutFree = true;
            }

            return isAnswer;
        }
    }  // namespace

    TuiShell::TuiShell(common::ThemeQuery theme)
        : interactiveScreen_(ftxui::ScreenInteractive::Fullscreen())
        , theme_(std::move(theme)) {}

    void TuiShell::run(
        common::PresentationContext& context,
        common::render::RendererSet& renderers,
        common::input::EventPipeline& pipeline,
        const common::host::SessionFinishedQuery& isFinished
    ) {
        const auto draw = [&context, &renderers] {
            return frameOfActiveScreen(context, renderers);
        };
        const auto route = [this, &context, &pipeline, &isFinished](const ftxui::Event& event) {
            return routeEvent(event, context, pipeline, isFinished);
        };

        blankAlternateScreen();
        askForKeyCodes();
        context.state().isKeyboardLayoutFree = platform::hostTranslatesKeyboardLayouts();

        interactiveScreen_.Loop(ftxui::CatchEvent(ftxui::Renderer(draw), route));
        tellTerminal(common::input::KEY_CODES_RELEASE);
    }

    ftxui::Element TuiShell::frameOfActiveScreen(
        const common::PresentationContext& context,
        common::render::RendererSet& renderers
    ) {
        const auto [width, height] = ftxui::Terminal::Size();
        const common::render::Frame frame =
            renderers.render(context.currentScreen(), width, height);

        return elementOfFrame(frame) | bgcolor(context.theme().background);
    }

    bool TuiShell::routeEvent(
        const ftxui::Event& event,
        common::PresentationContext& context,
        common::input::EventPipeline& pipeline,
        const common::host::SessionFinishedQuery& isFinished
    ) {
        if (isAboutKeyCodes(event, context)) {
            return true;
        }

        const common::input::Keystroke stroke = keystrokeOf(event);
        if (stroke.key == common::input::Key::Interrupt) {
            interactiveScreen_.PostEvent(ftxui::Event::CtrlC);
            return true;
        }

        pipeline.offer(stroke);
        const bool isClaimed = pipeline.settle();
        if (isFinished()) {
            interactiveScreen_.Exit();
        }

        return isClaimed;
    }
}  // namespace cpp_warships::head::tui
