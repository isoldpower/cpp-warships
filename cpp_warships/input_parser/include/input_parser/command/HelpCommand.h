#pragma once
#include <input_parser/model/Parser.h>
#include <input_parser/command/ParserCommand.h>

namespace cpp_warships::input_parser {
    class HelpCommand : public ParserCommand {
    private:
        SchemeMap<ParserCommand*> scheme;
    public:
        explicit HelpCommand(SchemeMap<ParserCommand*> scheme);

        void execute(ParsedOptions options) override;
    };
}