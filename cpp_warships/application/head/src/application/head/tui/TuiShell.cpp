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
        blankAlternateScreen();
        tellTerminal(common::input::KEY_CODES_REQUEST);
        tellTerminal(common::input::KEY_CODES_QUERY);
        context.state().isKeyboardLayoutFree = platform::hostTranslatesKeyboardLayouts();

        const auto renderActiveScreen = [&context, &renderers] {
            const auto [dimx, dimy] = ftxui::Terminal::Size();
            const common::render::Frame frame =
                renderers.render(context.currentScreen(), dimx, dimy);

            return elementOfFrame(frame) | bgcolor(context.theme().background);
        };

        const auto routeEvent =
            [this, &context, &pipeline, &isFinished](const ftxui::Event& event) {
                if (common::input::isKeyCodesAnswer(event.input())) {
                    context.state().isKeyboardLayoutFree = true;
                    return true;
                }
                if (common::input::keystrokeOfKeyCode(event.input()).has_value()) {
                    context.state().isKeyboardLayoutFree = true;
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
            };

        interactiveScreen_.Loop(ftxui::CatchEvent(ftxui::Renderer(renderActiveScreen), routeEvent));
        tellTerminal(common::input::KEY_CODES_RELEASE);
    }
}  // namespace cpp_warships::head::tui
