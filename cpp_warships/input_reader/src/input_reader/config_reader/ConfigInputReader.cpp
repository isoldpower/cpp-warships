#include <input_reader/config_reader/ConfigInputReader.h>
#include <input_reader/console_reader/ConsoleInputReader.h>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace cpp_warships::input_reader::config_reader {

    ConfigInputReader::ConfigInputReader(const std::string &filename)
        : fileContents({})
        , shadowReader(new console_reader::ConsoleInputReader())
        , linesExecuted(0)
    {
        std::ifstream file(filename);

        std::vector<std::string> fileContents;
        if (!file.is_open() || std::filesystem::is_directory(filename)) {
            std::cerr << "Error opening file!" << std::endl;
        } else {
            std::string line;
            while (std::getline(file, line)) {
                fileContents.push_back(line);
            }

            file.close();
        }

        this->fileContents = fileContents;
    }

    std::string ConfigInputReader::readCommand() {
        auto command = linesExecuted >= this->fileContents.size()
            ? shadowReader->readCommand()
            : this->fileContents[linesExecuted++];

        linesExecuted = std::min(linesExecuted, this->fileContents.size());

        return command;
    }

} // namespace cpp_warships::input_reader::config_reader