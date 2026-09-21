#include <input_reader/command_reader/CommandInputReader.h>

#include <utilities/StringHelper.h>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace cpp_warships::input_reader::command_reader {

    CommandInputReader::CommandInputReader(const std::string &filename, char delimiter)
        : keymap({})
    {
        std::ifstream file(filename);

        std::vector<std::string> fileContents;
        if (!file.is_open() || std::filesystem::is_directory(filename)) {
            std::cerr << "Error opening file!" << std::endl;
        } else {
            std::string line;
            while (std::getline(file, line)) {
                std::vector<std::string> chunks = StringHelper::split(line, delimiter);
                if (chunks.size() == 2) {
                    this->keymap[StringHelper::trim(chunks[0])] = StringHelper::trim(chunks[1]);
                }
            }

            file.close();
        }
    }

    std::string CommandInputReader::readCommand() {
        std::string input;
        std::getline(std::cin, input);

        bool isCommand = keymap.find(input) != keymap.end();
        if (isCommand) {
            return keymap[input];
        } else {
            return input;
        }
    }

} // namespace cpp_warships::input_reader::command_reader