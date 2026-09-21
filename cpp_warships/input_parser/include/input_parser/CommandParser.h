#pragma once

#include <utilities/TypesHelper.h>

#include <input_parser/command/ParserCommand.h>
#include <input_parser/model/Parser.h>


namespace cpp_warships::input_parser {
    using ParseResult = std::pair<ParseCallback<ParserCommand*>, ParsedOptions>;

    class CommandParser : Parser<ParserCommand*> {
    private:
        ParserCommand* printCommandsHelp(ParsedOptions options);
        ParserCommand* printCommandsError(ParsedOptions options);
        ParserCommand* printArgumentsError(ParserCommandInfo<ParserCommand *> command, ParsedOptions options);
    public:
        explicit CommandParser(const SchemeMap<ParserCommand*> &scheme);

        CommandParser(
            const SchemeMap<ParserCommand*> &scheme,
            const ParseCallback<void> &displayError,
            const SchemeHelpCallback<void> &printHelp = nullptr
        );

        ~CommandParser() override = default;

        void executedParse(const std::string &input) override;
        std::pair<ParseCallback<ParserCommand *>, ParsedOptions> getCommandError() override;
        BindedParseCallback<ParserCommand*> bindedParse(const std::string &input) override;
        std::pair<ParseCallback<ParserCommand *>, ParsedOptions> getOptionsError(
            ParserCommandInfo<ParserCommand *> command,
            ParsedOptions arguments
        ) override;
    };
}