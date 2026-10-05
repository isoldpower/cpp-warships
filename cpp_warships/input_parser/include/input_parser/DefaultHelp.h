#pragma once

#include <input_parser/model/ParserCommandInfo.h>
#include <input_parser/model/ParserParameter.h>
#include <utilities/ViewHelper.h>

#include <iostream>

namespace cpp_warships::input_parser {
    class DefaultHelp {
    public:
        static void PrintParameter(const model::ParserParameter& parameter);

        template <typename T>
        static void PrintCommand(
            std::pair<std::string, model::ParserCommandInfo<T>> command,
            std::function<void(model::ParserParameter)> printParameter
        ) {
            model::ParserCommandInfo currentCommand = command.second;
            ViewHelper::consoleOut("print '" + command.first + "': ", 1);
            ViewHelper::consoleOut("├── description: " + currentCommand.getDescription(), 1);

            const std::vector<model::ParserParameter> parameters = currentCommand.getParameters();
            if (parameters.empty()) {
                ViewHelper::consoleOut("└── params: empty", 1);
            } else {
                ViewHelper::consoleOut("└── params:", 1);
            }

            for (std::size_t position = 0; position < parameters.size(); ++position) {
                ViewHelper::consoleOut("Param (" + std::to_string(position + 1) + ")", 2);
                printParameter(parameters[position]);
            }
        }
    };
}  // namespace cpp_warships::input_parser