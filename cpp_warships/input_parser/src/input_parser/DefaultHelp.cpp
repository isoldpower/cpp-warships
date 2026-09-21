#include <input_parser/DefaultHelp.h>

#include <utilities/ViewHelper.h>
#include <iostream>

namespace cpp_warships::input_parser {
    void DefaultHelp::PrintParam(const ParserParameter &param) {
        std::string flagsOutput;
        for (const auto& flag : param.getFlags()) {
            flagsOutput += flag + " ";
        }
        ViewHelper::consoleOut("├── flags: [ " + flagsOutput + "]", 2);

        std::string description = param.getDescription().empty() ? "none" : param.getDescription();
        ViewHelper::consoleOut("├── description: " + description, 2);
        std::string necessary = param.getNecessary() ? "true" : "false";
        ViewHelper::consoleOut("└── is necessary: " + necessary, 2);
    };
}