#include <cpp_warships/utilities/include/Initials.h>
#include <cpp_warships/input_reader/include/config_reader/ConfigInputReader.h>

#include "include/GameController.h"

using namespace cpp_warships;

int main() {
    Initials::consoleOutInitials();

    const std::string filename = "../test.txt";
    auto* input_reader = new input_reader::config_reader::ConfigInputReader(filename);
    application::GameController controller(input_reader);

    controller.run();

    return 0;
}
