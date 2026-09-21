#pragma once
#include <input_parser/builder/ConfigCommandBuilder.h>


namespace cpp_warships::input_parser {
    template <typename T>
    class ParserBuilderDirector {
    private:
        ParserCommandBuilder<T>& commandBuilder;
    public:
        explicit ParserBuilderDirector(ParserCommandBuilder<T> &commandBuilder)
            : commandBuilder(commandBuilder)
        {}
    };
}