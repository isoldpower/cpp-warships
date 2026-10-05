#pragma once

#include <input_parser/command/ParserCommand.h>
#include <input_parser/model/Parser.h>
#include <utilities/TypesHelper.h>

namespace cpp_warships::input_parser {
    using ParseResult =
        std::pair<model::ParseCallback<command::ParserCommand*>, model::ParsedOptions>;

    class CommandParser : model::Parser<command::ParserCommand*> {
    private:
        command::ParserCommand* printCommandsHelp(model::ParsedOptions options);
        command::ParserCommand* printCommandsError(model::ParsedOptions options);
        command::ParserCommand* printArgumentsError(
            model::ParserCommandInfo<command::ParserCommand*> command,
            model::ParsedOptions options
        );

    public:
        explicit CommandParser(const model::SchemeMap<command::ParserCommand*>& commandScheme);

        CommandParser(
            const model::SchemeMap<command::ParserCommand*>& commandScheme,
            const model::ParseCallback<void>& errorDisplay,
            const model::SchemeHelpCallback<command::ParserCommand*>& printHelp = nullptr
        );

        ~CommandParser() override = default;

        void executedParse(const std::string& input) override;
        std::pair<model::ParseCallback<command::ParserCommand*>, model::ParsedOptions>
        getCommandError() override;
        model::BindedParseCallback<command::ParserCommand*> bindedParse(
            const std::string& input
        ) override;
        std::pair<model::ParseCallback<command::ParserCommand*>, model::ParsedOptions>
        getOptionsError(
            model::ParserCommandInfo<command::ParserCommand*> command,
            model::ParsedOptions arguments
        ) override;
    };
}  // namespace cpp_warships::input_parser