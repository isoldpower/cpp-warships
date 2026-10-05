#include <input_parser/CommandParser.h>
#include <input_parser/builder/ConfigCommandBuilder.h>
#include <input_parser/command/ArgumentsErrorCommand.h>
#include <input_parser/command/CallbackCommand.h>
#include <input_parser/command/ErrorCommand.h>
#include <input_parser/command/HelpCommand.h>

#include <stdexcept>

namespace cpp_warships::input_parser {
    command::ParserCommand* CommandParser::printCommandsHelp(model::ParsedOptions) {
        return new command::HelpCommand(this->scheme);
    }

    command::ParserCommand* CommandParser::printCommandsError(model::ParsedOptions) {
        return new command::ErrorCommand(this->displayError);
    }

    command::ParserCommand* CommandParser::printArgumentsError(
        model::ParserCommandInfo<command::ParserCommand*> command,
        model::ParsedOptions
    ) {
        return new command::ArgumentsErrorCommand(command);
    }

    CommandParser::CommandParser(const model::SchemeMap<command::ParserCommand*>& commandScheme)
        : model::Parser<command::ParserCommand*>(commandScheme) {}

    CommandParser::CommandParser(
        const model::SchemeMap<command::ParserCommand*>& commandScheme,
        const model::ParseCallback<void>& errorDisplay,
        const model::SchemeHelpCallback<command::ParserCommand*>& printHelp
    )
        : model::Parser<command::ParserCommand*>(commandScheme, errorDisplay) {
        if (!commandScheme.contains("help")) {
            builder::ConfigCommandBuilder<command::ParserCommand*> commandBuilder;
            model::ParserCommandInfo<command::ParserCommand*>* helpInfo;

            model::ParseCallback<command::ParserCommand*> help;
            if (printHelp) {
                const auto renderScheme = [this, printHelp](model::ParsedOptions) {
                    printHelp(this->scheme);
                };
                help = [renderScheme](model::ParsedOptions) -> command::ParserCommand* {
                    return new command::CallbackCommand(renderScheme);
                };
            } else {
                help = TypesHelper::methodToFunction(&CommandParser::printCommandsHelp, this);
            }

            helpInfo = new model::ParserCommandInfo<command::ParserCommand*>(
                {commandBuilder.setDescription("command::Command to display this message")
                     .setCallback(help)
                     .buildAndReset()}
            );

            this->scheme.insert({"help", *helpInfo});
            delete helpInfo;
        }
    }

    void CommandParser::executedParse(const std::string& input) {
        ParseResult parseResult = this->parse(input);
        if (parseResult.first) {
            command::ParserCommand* command = parseResult.first(parseResult.second);
            command->execute(parseResult.second);
        }
    }

    model::BindedParseCallback<command::ParserCommand*> CommandParser::bindedParse(
        const std::string& input
    ) {
        ParseResult result = this->parse(input);
        return std::bind(result.first, result.second);
    }

    std::pair<model::ParseCallback<command::ParserCommand*>, model::ParsedOptions> CommandParser::
        getOptionsError(
            model::ParserCommandInfo<command::ParserCommand*> command,
            model::ParsedOptions arguments
        ) {
        model::ParseCallback<command::ParserCommand*> protectedDisplayError =
            command.getErrorDisplay() ? std::bind(
                                            &CommandParser::printArgumentsError,
                                            this,
                                            command,
                                            std::placeholders::_1
                                        )
                                      : throw std::invalid_argument(
                                            "Arguments validation failed. You can "
                                            "get better error message "
                                            "by providing displayError callback"
                                        );

        return std::make_pair(protectedDisplayError, arguments);
    }

    std::pair<model::ParseCallback<command::ParserCommand*>, model::ParsedOptions> CommandParser::
        getCommandError() {
        model::ParseCallback<command::ParserCommand*> commandNotFound =
            this->displayError
                ? TypesHelper::methodToFunction(&CommandParser::printCommandsError, this)
                : throw std::invalid_argument(
                      "command::Command not found. You can get better error "
                      "message by providing displayError callback"
                  );

        return std::make_pair(commandNotFound, model::ParsedOptions());
    }
}  // namespace cpp_warships::input_parser