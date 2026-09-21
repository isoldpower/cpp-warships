#pragma once
#include <input_parser/model/ParserCommandInfo.h>


namespace cpp_warships::input_parser {
    template <typename T>
    class ParserCommandBuilder {
    protected:
        std::string description;
        std::vector<ParserParameter> parameters;
        ParseCallback<T> executable;
        ParseCallback<void> displayError;
        ParseCallback<void> printHelp;
        bool resolveAllFlags = false;

    public:
        virtual ~ParserCommandBuilder() = default;

        virtual ParserCommandBuilder& setDescription(std::string description) = 0;
        virtual ParserCommandBuilder& addParameter(ParserParameter parameter) = 0;
        virtual ParserCommandBuilder& setDisplayError(ParseCallback<void> displayError) = 0;
        virtual ParserCommandBuilder& setCallback(ParseCallback<T> function) = 0;
        virtual ParserCommandBuilder& setPrintHelp(ParseCallback<void> help) = 0;
        virtual ParserCommandBuilder& setResolveAllFlags(bool resolveAll) = 0;
        void reset() {
            description = "";
            parameters.clear();
            executable = nullptr;
            displayError = nullptr;
            printHelp = nullptr;
            setResolveAllFlags(false);
        }
    };
}