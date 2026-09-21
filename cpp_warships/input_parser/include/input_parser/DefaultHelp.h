#pragma once

#include <utilities/ViewHelper.h>
#include <iostream>

#include <input_parser/model/ParserCommandInfo.h>
#include <input_parser/model/ParserParameter.h>

namespace cpp_warships::input_parser {
    class DefaultHelp {
    public:
        static void PrintParam(const ParserParameter &param);

        template <typename T>
        static void PrintCommand(
            std::pair<std::string, ParserCommandInfo<T>> command,
            std::function<void(ParserParameter)> printParam
        ) {
            ParserCommandInfo currentCommand = command.second;
            ViewHelper::consoleOut("print '" + command.first + "': ", 1);
            ViewHelper::consoleOut("├── description: " + currentCommand.getDescription(), 1);

            std::vector<ParserParameter> params = currentCommand.getParams();
            if (params.empty()) {
                ViewHelper::consoleOut("└── params: empty", 1);
            } else {
                ViewHelper::consoleOut("└── params:", 1);
            }

            for (int i = 0; i < static_cast<int>(params.size()); i++) {
                ViewHelper::consoleOut("Param (" + std::to_string(i + 1) + ")", 2);
                printParam(params[i]);
            }
        }
    };
}