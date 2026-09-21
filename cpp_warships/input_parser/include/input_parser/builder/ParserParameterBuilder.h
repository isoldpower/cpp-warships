#pragma once
#include <regex>


namespace cpp_warships::input_parser {
    class ParserParameterBuilder {
    protected:
        std::vector<std::string> flags;
        std::regex validator;
        std::string description;
        bool necessary = false;

    public:
        virtual ~ParserParameterBuilder() = default;

        virtual ParserParameterBuilder& addFlag(std::string flag) = 0;
        virtual ParserParameterBuilder& setValidator(std::regex validator) = 0;
        virtual ParserParameterBuilder& setDescription(std::string description) = 0;
        virtual ParserParameterBuilder& setNecessary(bool necessary) = 0;
        void reset() {
            flags.clear();
            validator = ".";
            description = "";
            necessary = false;
        };
    };
}
