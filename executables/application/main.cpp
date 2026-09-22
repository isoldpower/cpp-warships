#include <game_flow/RandomEngine.h>
#include <game_tui/Application.h>
#include <utilities/Initials.h>

auto main() -> int {
    Initials::consoleOutInitials();

    cpp_warships::game_flow::RandomEngine randomEngine =
            cpp_warships::game_flow::makeRandomlySeededEngine();
    cpp_warships::game_tui::Application application{randomEngine};

    application.run();

    return 0;
}
