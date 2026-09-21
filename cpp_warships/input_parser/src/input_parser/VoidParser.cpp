#include <input_parser/model/Parser.h>
#include <input_parser/model/ParserCommandInfo.h>
#include <input_parser/builder/ConfigCommandBuilder.h>
#include <input_parser/VoidParser.h>
#include <input_parser/DefaultHelp.h>

#include <utilities/TypesHelper.h>
#include <utilities/ViewHelper.h>


namespace cpp_warships::input_parser {
    void VoidParser::printCommandsHelp(ParsedOptions options) {
        ViewHelper::consoleOut("This is the list of supported commands:");

        for (const auto& command : scheme) {
            auto commandPrint = command.second.getPrintHelp();
            if (commandPrint) {
                commandPrint(options);
            } else {
                DefaultHelp::PrintCommand<void>(command, DefaultHelp::PrintParam);
                ViewHelper::consoleOut("");
            }
        }
    }

    VoidParser::VoidParser(const SchemeMap<void> &scheme)
        : VoidParser(scheme, nullptr, nullptr)
    {}

    VoidParser::VoidParser(
            const SchemeMap<void> &scheme,
            const ParseCallback<void> &displayError,
            const SchemeHelpCallback<void> &printHelp
    )
        : Parser<void>(scheme, displayError, printHelp)
    {
        if (scheme.find("help") == scheme.end()) {
            ConfigCommandBuilder<void> commandBuilder;
            ParserCommandInfo<void>* helpInfo;

            if (printHelp) {
                helpInfo = new ParserCommandInfo<void>({
                    commandBuilder
                        .setDescription("Command to display this message")
                        .setCallback(std::bind(printHelp, scheme))
                        .buildAndReset()
                });
            } else {
                helpInfo = new ParserCommandInfo<void>({
                    commandBuilder
                        .setDescription("Command to display this message")
                        .setCallback(TypesHelper::methodToFunction(&VoidParser::printCommandsHelp, this))
                        .buildAndReset()
                });
            }

            this->scheme.insert({"help", *helpInfo});
            delete helpInfo;
        }}

    BindedParseCallback<void> VoidParser::bindedParse(const std::string &input) {
        std::pair<ParseCallback<void>, ParsedOptions> result = this->parse(input);
        return std::bind(result.first, result.second);
    }

    void VoidParser::executedParse(const std::string &input) {
        std::pair<ParseCallback<void>, ParsedOptions> result = this->parse(input);
        result.first(result.second);
    }

    std::pair<ParseCallback<void>, ParsedOptions> VoidParser::getCommandError() {
        ParseCallback<void> commandNotFound = this->displayError
                    ? this->displayError
                    : throw std::invalid_argument("Command not found. You can get better error message by providing displayError callback");

        return std::make_pair(commandNotFound, ParsedOptions());
    }

    std::pair<ParseCallback<void>, ParsedOptions> VoidParser::getOptionsError(ParserCommandInfo<void> command, ParsedOptions arguments) {
        ParseCallback<void> protectedDisplayError = command.getErrorDisplay()
            ? command.getErrorDisplay()
            : throw std::invalid_argument("Arguments validation failed. You can get better error message by providing displayError callback");

        return std::make_pair(protectedDisplayError, arguments);
    }
}