#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <utilities/Initials.h>

// Placeholder shell for the terminal UI rewrite.
// Replaced by cpp_warships::game_tui::App once the TUI layer lands.
auto main() -> int {
    Initials::consoleOutInitials();

    auto screen = ftxui::ScreenInteractive::TerminalOutput();

    const auto placeholder = ftxui::Renderer([] {
        return ftxui::vbox({
                   ftxui::text("cpp-warships") | ftxui::bold | ftxui::hcenter,
                   ftxui::separator(),
                   ftxui::text("Terminal UI rewrite in progress.") | ftxui::hcenter,
                   ftxui::text("Press q to quit.") | ftxui::dim | ftxui::hcenter,
               }) |
               ftxui::border | ftxui::size(ftxui::WIDTH, ftxui::GREATER_THAN, 40);
    });

    const auto root = ftxui::CatchEvent(placeholder, [&](const ftxui::Event& event) {
        if (event == ftxui::Event::Escape ||
            (event.is_character() && event.character() == "q")) {
            screen.Exit();
            return true;
        }
        return false;
    });

    screen.Loop(root);

    return 0;
}
