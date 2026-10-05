#pragma once
#include <input_parser/model/ParserCommandInfo.h>
#include <utilities/StringHelper.h>

#include <algorithm>
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

        void increaseAmountOnHashMap(
            const std::string& key,
            std::unordered_map<std::string, int>& map
        ) {
            ++map[key];
        }

        void decreaseAmountOnHashMap(
            const std::string& key,
            std::unordered_map<std::string, int>& map
        ) {
            const auto found = map.find(key);
            if (found != map.end()) {
                --found->second;
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
            for (const ParserParameter& parameter : command.getParameters()) {
                if (parameter.getIsFlagPresent(flag)) {
                    result = parameter;
                    return true;
                }
            }

            return false;
        }

        /** @brief How many times each flag of a command may appear, and how many of those
         * appearances are required. */
        struct FlagCounts {
            std::unordered_map<std::string, int> necessary;
            std::unordered_map<std::string, int> all;
        };

        FlagCounts flagCountsOf(const ParserCommandInfo<T>& commandScheme) {
            FlagCounts counts;
            for (const auto& parameter : commandScheme.getParameters()) {
                for (const std::string& flag : parameter.getFlags()) {
                    increaseAmountOnHashMap(flag, counts.all);
                    if (parameter.getNecessary()) {
                        increaseAmountOnHashMap(flag, counts.necessary);
                    }
                }
            }

            return counts;
        }

        bool necessaryFlagsPresent(
            const std::vector<std::string>& input,
            const ParserCommandInfo<T>& commandScheme
        ) {
            const auto isStillMissing = [](const auto& entry) { return entry.second > 0; };
            const auto isRepeated = [](const auto& entry) { return entry.second < 0; };

            FlagCounts counts = flagCountsOf(commandScheme);
            for (const std::string& chunk : input) {
                decreaseAmountOnHashMap(chunk, counts.necessary);
                decreaseAmountOnHashMap(chunk, counts.all);
            }

            return std::none_of(counts.necessary.begin(), counts.necessary.end(), isStillMissing) &&
                   std::none_of(counts.all.begin(), counts.all.end(), isRepeated);
        }

        std::pair<bool, ParsedOptions> validateParameters(
            const std::vector<std::string>& inputChunks,
            ParserCommandInfo<T>& command
        ) {
            bool isValid = true;
            ParsedOptions validValues;

            for (std::size_t position = 0; position < inputChunks.size(); ++position) {
                const std::string& chunk = inputChunks[position];
                if (chunk.substr(0, 2) != "--") {
                    continue;
                }

                ParserParameter option;
                if (!findOption(chunk, command, option)) {
                    isValid = isValid && !command.getResolveAllFlags();
                    continue;
                }

                const bool isLastChunk = position + 1 == inputChunks.size();
                const std::string optionValue = isLastChunk ? "" : inputChunks[position + 1];
                const std::pair<bool, std::string> validationResult = option.validate(optionValue);
                isValid = isValid && validationResult.first;
                if (isValid) {
                    validValues.emplace(chunk.substr(2), validationResult.second);
                }
            }

            return std::make_pair(isValid, validValues);
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
                this->validateParameters(splitInput, relatedCommand);
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