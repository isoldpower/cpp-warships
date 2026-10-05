#pragma once
#include <input_parser/model/Parser.h>

#include <functional>

namespace cpp_warships::input_parser {
    class VoidParser : model::Parser<> {
    private:
        void printCommandsHelp(model::ParsedOptions options);

    public:
        explicit VoidParser(const model::SchemeMap<void>& commandScheme);

        VoidParser(
            const model::SchemeMap<void>& commandScheme,
            const model::ParseCallback<void>& errorDisplay,
            const model::SchemeHelpCallback<void>& printHelp = nullptr
        );

        model::BindedParseCallback<void> bindedParse(const std::string& input) override;
        void executedParse(const std::string& input) override;
        std::pair<model::ParseCallback<void>, model::ParsedOptions> getCommandError() override;
        std::pair<model::ParseCallback<void>, model::ParsedOptions> getOptionsError(
            model::ParserCommandInfo<void> command,
            model::ParsedOptions arguments
        ) override;
    };
}  // namespace cpp_warships::input_parser