#include <input_parser/DefaultHelp.h>
#include <utilities/ViewHelper.h>

#include <iostream>

namespace cpp_warships::input_parser {
    void DefaultHelp::PrintParameter(const model::ParserParameter& parameter) {
        std::string flagsOutput;
        for (const auto& flag : parameter.getFlags()) {
            flagsOutput += flag + " ";
        }
        ViewHelper::consoleOut("├── flags: [ " + flagsOutput + "]", 2);

        std::string description =
            parameter.getDescription().empty() ? "none" : parameter.getDescription();
        ViewHelper::consoleOut("├── description: " + description, 2);
        std::string necessary = parameter.getNecessary() ? "true" : "false";
        ViewHelper::consoleOut("└── is necessary: " + necessary, 2);
    };
}  // namespace cpp_warships::input_parser