#pragma once
#include <utility>

#include <input_parser/command/ParserCommand.h>


namespace cpp_warships::input_parser {
    class ArgumentsErrorCommand: public ParserCommand {
    private:
        ParserCommandInfo<ParserCommand *> command;
    public:
        explicit ArgumentsErrorCommand(ParserCommandInfo<ParserCommand *> command)
            : ParserCommand()
            , command(std::move(command))
        {}

        void execute(ParsedOptions options) override;
    };
}