#pragma once
#include <map>

#include <input_parser/model/ParserCommandInfo.h>


namespace cpp_warships::input_parser {
    template <typename T>
    class Command {
    public:
        virtual ~Command() = default;

        virtual void execute(T data) = 0;
    };
}