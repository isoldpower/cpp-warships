#pragma once
#include <input_parser/model/ParserCommandInfo.h>
#include <utilities/StringHelper.h>

#include <functional>
#include <map>
#include <unordered_map>
#include <utility>

namespace cpp_warships::input_parser::model {
    template <typename T>
    using SchemeMap = std::map<std::string, ParserCommandInfo<T>>;

    template <typename T>
    using SchemeHelpCallback = std::function<void(SchemeMap<T>)>;

    template <typename T = void>
    class Parser {
    protected:
        SchemeMap<T> scheme;
        ParseCallback<void> displayError;

        void increaseAmountOnHashMap(std::string& key, std::unordered_map<std::string, int>& map) {
            if (map.find(key) == map.end()) {
                map[key] = 1;
            } else {
                map[key]++;
            }
        }

        void decreaseAmountOnHashMap(std::string& key, std::unordered_map<std::string, int>& map) {
            if (map.find(key) != map.end()) {
                map[key]--;
            }
        }

        bool commandInScheme(std::string& command, const SchemeMap<T>& searchedScheme) {
            return searchedScheme.find(command) != searchedScheme.end();
        }

        bool findOption(
            const std::string& flag,
            const ParserCommandInfo<T>& command,
            ParserParameter& result
        ) {
            for (size_t i = 0; i < command.getParams().size(); i++) {
                ParserParameter param = command.getParams()[i];
                if (param.getIsFlagPresent(flag)) {
                    result = param;
                    return true;
                }
            }

            return false;
        }

        bool necessaryFlagsPresent(
            const std::vector<std::string>& input,
            const ParserCommandInfo<T>& commandScheme
        ) {
            auto params = commandScheme.getParams();

            std::unordered_map<std::string, int> necessaryFlags;
            std::unordered_map<std::string, int> allFlags;

            for (const auto& param : params) {
                std::vector<std::string> flags = param.getFlags();
                bool isNecessary = param.getNecessary();

                for (size_t j = 0; j < flags.size(); j++) {
                    increaseAmountOnHashMap(flags[j], allFlags);
                    if (isNecessary)
                        increaseAmountOnHashMap(flags[j], necessaryFlags);
                }
            }

            for (auto currentChunk : input) {
                decreaseAmountOnHashMap(currentChunk, necessaryFlags);
                decreaseAmountOnHashMap(currentChunk, allFlags);
            }

            for (auto& necessaryFlag : necessaryFlags) {
                if (necessaryFlag.second > 0)
                    return false;
            }

            for (auto& allFlag : allFlags) {
                if (allFlag.second < 0)
                    return false;
            }

            return true;
        }

        std::pair<bool, ParsedOptions> validateParams(
            const std::vector<std::string>& inputChunks,
            ParserCommandInfo<T>& command
        ) {
            bool isValid = true;
            ParsedOptions validParamValues;

            for (size_t i = 0; i < inputChunks.size(); i++) {
                const std::string& chunk = inputChunks[i];
                if (chunk.substr(0, 2) != "--") {
                    continue;
                }

                ParserParameter option;
                if (!findOption(chunk, command, option)) {
                    isValid = isValid && !command.getResolveAllFlags();
                    continue;
                }

                const std::string optionValue =
                    i == inputChunks.size() - 1 ? "" : inputChunks[i + 1];
                const std::pair<bool, std::string> validationResult = option.validate(optionValue);
                isValid = isValid && validationResult.first;
                if (isValid) {
                    validParamValues.emplace(chunk.substr(2), validationResult.second);
                }
            }

            return std::make_pair(isValid, validParamValues);
        }

    public:
        virtual ~Parser() = default;

        explicit Parser(SchemeMap<T> scheme)
            : scheme(std::move(scheme))
            , displayError(nullptr) {}

        explicit Parser(
            SchemeMap<T> scheme,
            ParseCallback<void> displayError,
            const SchemeHelpCallback<void>& = nullptr
        )
            : scheme(std::move(scheme))
            , displayError(std::move(displayError)) {}

        std::pair<ParseCallback<T>, ParsedOptions> parse(const std::string& input) {
            std::vector<std::string> splitInput = StringHelper::split(input, ' ');
            if (splitInput.empty() || !commandInScheme(splitInput[0], this->scheme) ||
                !necessaryFlagsPresent(splitInput, this->scheme.at(splitInput[0]))) {
                return getCommandError();
            }

            ParserCommandInfo<T> relatedCommand = this->scheme.at(splitInput[0]);
            const std::pair<bool, ParsedOptions> validationResult =
                this->validateParams(splitInput, relatedCommand);
            ParsedOptions parsedArguments = validationResult.second;

            if (!validationResult.first) {
                return getOptionsError(relatedCommand, parsedArguments);
            }

            return std::make_pair(relatedCommand.getExecutable(), parsedArguments);
        }

        virtual std::pair<ParseCallback<T>, ParsedOptions> getCommandError() = 0;
        virtual std::pair<ParseCallback<T>, ParsedOptions> getOptionsError(
            ParserCommandInfo<T> command,
            ParsedOptions arguments
        ) = 0;
        virtual BindedParseCallback<T> bindedParse(const std::string& input) = 0;
        virtual void executedParse(const std::string& input) = 0;
    };
}  // namespace cpp_warships::input_parser::model