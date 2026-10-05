#include <application/head/common/PresentationContext.h>
#include <application/head/common/input/EventPipeline.h>
#include <application/head/common/render/RendererSet.h>
#include <application/head/plain/TerminalShell.h>

#include <algorithm>
#include <cctype>
#include <istream>
#include <map>
#include <ostream>
#include <string>

namespace cpp_warships::head::plain {
    namespace {
        const std::string PROMPT = "> ";

        /** @brief Whether @p command is a lone space: a name may hold one, so it is a keystroke
         * of its own rather than a blank line asking for Enter. */
        [[nodiscard]] bool isTypedSpace(const std::string& command) {
            return command == " ";
        }

        /** @brief The typed word stripped of surrounding blanks and folded to
         * lower case, so that "  Up " and "up" ask for the same thing. */
        [[nodiscard]] std::string normalised(const std::string& command) {
            const auto isBlank = [](const unsigned char letter) {
                return std::isspace(letter) != 0;
            };
            const auto toLower = [](const unsigned char letter) {
                return static_cast<char>(std::tolower(letter));
            };

            if (isTypedSpace(command)) {
                return command;
            }

            const auto first = std::find_if_not(command.begin(), command.end(), isBlank);
            const auto last = std::find_if_not(command.rbegin(), command.rend(), isBlank).base();

            if (first >= last) {
                return {};
            }
            std::string trimmed(first, last);
            std::transform(trimmed.begin(), trimmed.end(), trimmed.begin(), toLower);
            return trimmed;
        }

        /** @brief The keystroke @p command stands for: a name for the keys that
         * cannot be typed as text, and otherwise the first letter typed. */
        [[nodiscard]] common::input::Keystroke keystrokeFromCommand(const std::string& command) {
            static const std::map<std::string, common::input::Key> NAMED_KEYS = {
                {"", common::input::Key::Enter},
                {"enter", common::input::Key::Enter},
                {"up", common::input::Key::ArrowUp},
                {"down", common::input::Key::ArrowDown},
                {"left", common::input::Key::ArrowLeft},
                {"right", common::input::Key::ArrowRight},
                {"esc", common::input::Key::Escape},
                {"tab", common::input::Key::Tab},
                {"back", common::input::Key::Backspace},
                {"pgup", common::input::Key::PageUp},
                {"pgdn", common::input::Key::PageDown},
            };

            const auto namedKey = NAMED_KEYS.find(command);
            if (namedKey != NAMED_KEYS.end()) {
                return common::input::Keystroke{.key = namedKey->second};
            }

            return common::input::Keystroke{
                .key = common::input::Key::Character,
                .character = command.substr(0, 1)
            };
        }
    }  // namespace

    TerminalShell::TerminalShell(std::istream& input, std::ostream& output)
        : input_(input)
        , output_(output) {}

    void TerminalShell::run(
        common::PresentationContext& context,
        common::render::RendererSet& renderers,
        common::input::EventPipeline& pipeline,
        const common::host::SessionFinishedQuery& isFinished
    ) {
        while (!isFinished()) {
            drawFrame(renderers, context.currentScreen());

            const std::optional<common::input::Keystroke> stroke = readKeystroke();
            if (!stroke.has_value()) {
                return;
            }

            pipeline.offer(*stroke);
            pipeline.settle();
        }
    }

    void TerminalShell::drawFrame(
        common::render::RendererSet& renderers,
        const common::ScreenKind screen
    ) const {
        output_ << common::render::frameToText(renderers.render(screen, 0, 0)) << PROMPT;
        output_.flush();
    }

    std::optional<common::input::Keystroke> TerminalShell::readKeystroke() const {
        std::string command;
        if (!std::getline(input_, command)) {
            return std::nullopt;
        }

        return keystrokeFromCommand(normalised(command));
    }
}  // namespace cpp_warships::head::plain
