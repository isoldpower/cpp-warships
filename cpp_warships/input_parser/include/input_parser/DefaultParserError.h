#pragma once
#include <input_parser/model/ParserCommandInfo.h>

namespace cpp_warships::input_parser {
    class DefaultParserError {
    public:
        static void CommandNotFoundError(ParsedOptions options);
        static void WrongFlagValueError(ParsedOptions options);
    };
}