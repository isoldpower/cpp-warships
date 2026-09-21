#pragma once
#include <functional>

#include <input_parser/model/Parser.h>

namespace cpp_warships::input_parser {
    class VoidParser : Parser<> {
    private:
        void printCommandsHelp(ParsedOptions options);
    public:
        explicit VoidParser(const SchemeMap<void> &scheme);

        VoidParser(
            const SchemeMap<void> &scheme, const ParseCallback<void> &displayError,
            const SchemeHelpCallback<void> &printHelp = nullptr
        );

        BindedParseCallback<void> bindedParse(const std::string &input) override;
        void executedParse(const std::string &input) override;
        std::pair<ParseCallback<void>, ParsedOptions> getCommandError() override;
        std::pair<ParseCallback<void>, ParsedOptions> getOptionsError(ParserCommandInfo<void> command, ParsedOptions arguments) override;
    };
}