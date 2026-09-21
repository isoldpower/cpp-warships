#pragma once

#include <map>
#include <string>

#include <input_reader/InputReader.h>

namespace cpp_warships::input_reader::command_reader {

    class CommandInputReader : public InputReader<> {
    private:
        std::map<std::string, std::string> keymap;
    public:
        explicit CommandInputReader(const std::string &filename, char delimiter = ';');
        std::string readCommand() override;
    };
} // namespace cpp_warships::input_reader::command_reader
