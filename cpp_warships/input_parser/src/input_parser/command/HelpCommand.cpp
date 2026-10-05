#include <input_parser/DefaultHelp.h>
#include <input_parser/command/HelpCommand.h>
#include <utilities/ViewHelper.h>

#include <utility>

namespace cpp_warships::input_parser::command {
    HelpCommand::HelpCommand(model::SchemeMap<ParserCommand*> scheme)
        : ParserCommand()
        , scheme(std::move(scheme)) {}

    void HelpCommand::execute(model::ParsedOptions options) {
        ViewHelper::consoleOut("This is the list of supported commands:");

        for (const auto& command : scheme) {
            auto commandPrint = command.second.getPrintHelp();

            if (commandPrint) {
                commandPrint(options);
            } else {
                DefaultHelp::PrintCommand<ParserCommand*>(command, DefaultHelp::PrintParameter);
                ViewHelper::consoleOut("");
            }
        }
    }
}  // namespace cpp_warships::input_parser::command