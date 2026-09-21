#include <input_parser/command/HelpCommand.h>
#include <input_parser/DefaultHelp.h>

#include <utilities/ViewHelper.h>

namespace cpp_warships::input_parser {
    HelpCommand::HelpCommand(SchemeMap<ParserCommand*> scheme)
        : ParserCommand()
        , scheme(std::move(scheme))
    {}

    void HelpCommand::execute(ParsedOptions options) {
        ViewHelper::consoleOut("This is the list of supported commands:");

        for (const auto& command : scheme) {
            auto commandPrint = command.second.getPrintHelp();

            if (commandPrint) {
                commandPrint(options);
            } else {
                DefaultHelp::PrintCommand<ParserCommand*>(command, DefaultHelp::PrintParam);
                ViewHelper::consoleOut("");
            }
        }
    }
}