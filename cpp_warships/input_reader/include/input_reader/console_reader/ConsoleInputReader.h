#pragma once

#include <string>

#include <input_reader/InputReader.h>

namespace cpp_warships::input_reader::console_reader {

    class ConsoleInputReader : public InputReader<> {
    public:
        std::string readCommand() override;
    };
} // namespace cpp_warships::input_reader::console_reader
