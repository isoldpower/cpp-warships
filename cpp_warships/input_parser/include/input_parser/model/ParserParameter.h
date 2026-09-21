#pragma once
#include <regex>
#include <vector>
#include <string>


namespace cpp_warships::input_parser {
    class ParserParameter {
    private:
        std::regex validator;
        std::string description;
        std::vector<std::string> flags;
        bool necessary = false;
    public:
        ParserParameter() = default;
        ParserParameter(std::vector<std::string> flags, std::regex validator, std::string description, bool necessary)
            : validator(std::move(validator))
            , description(std::move(description))
            , flags(std::move(flags))
            , necessary(necessary)
        {};

        [[nodiscard]] std::string getDescription() const {
            return this->description;
        }

        [[nodiscard]] bool getNecessary() const {
            return this->necessary;
        }

        [[nodiscard]] bool getIsFlagPresent(const std::string &flag) const {
            for (int i = 0; i < static_cast<int>(this->flags.size()); i++) {
                if (this->flags[i] == flag) {
                    return true;
                }
            }

            return false;
        }

        [[nodiscard]] std::vector<std::string> getFlags() const {
            return this->flags;
        }

        [[nodiscard]] std::pair<bool, std::string> validate(const std::string& input) const {
            bool isValid = std::regex_match(input, this->validator);

            return std::make_pair(isValid, input);
        }

    };
}