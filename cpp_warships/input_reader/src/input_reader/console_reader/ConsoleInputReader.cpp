#include <input_reader/console_reader/ConsoleInputReader.h>

#include <iostream>

namespace cpp_warships::input_reader::console_reader {

    std::string ConsoleInputReader::readCommand() {
        std::string command;
        std::getline(std::cin, command);
        return command;
    }
} // namespace cpp_warships::input_reader::console_reader