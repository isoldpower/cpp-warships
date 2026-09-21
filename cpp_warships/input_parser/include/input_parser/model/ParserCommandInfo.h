#pragma once
#include <iostream>
#include <map>
#include <string>

#include <input_parser/model/ParserParameter.h>


namespace cpp_warships::input_parser {
    typedef std::map<std::string, std::string> ParsedOptions;

    template <typename T>
    using ParseCallback = std::function<T(ParsedOptions)>;


    template <typename T>
    using BindedParseCallback = std::function<T()>;


    template <typename T>
    struct ParserCommandInfoConfig {
        ParserCommandInfoConfig(
            std::string description,
            std::vector<ParserParameter> parameters,
            ParseCallback<T> function,
            ParseCallback<void> displayError,
            bool resolveAllFlags,
            ParseCallback<void> printHelp
        )
            : resolveAllFlags(resolveAllFlags)
            , description(std::move(description))
            , parameters(std::move(parameters))
            , executable(std::move(function))
            , displayError(std::move(displayError))
            , printHelp(std::move(printHelp))
        {}

        ParserCommandInfoConfig(
            std::string description,
            std::vector<ParserParameter> parameters,
            ParseCallback<T> function
        )
            : description(std::move(description))
            , parameters(std::move(parameters))
            , executable(std::move(function))
        {}

        bool resolveAllFlags = false;
        std::string description;
        std::vector<ParserParameter> parameters;
        ParseCallback<T> executable;
        ParseCallback<void> displayError;
        ParseCallback<void> printHelp;
    };

    template<typename T>
    class ParserCommandInfo {
    private:
        ParserCommandInfoConfig<T> config;
    public:
        explicit ParserCommandInfo(ParserCommandInfoConfig<T> config)
            : config(std::move(config))
        {}
        [[nodiscard]] std::string getDescription() const {
            return this->config.description;
        }

        [[nodiscard]] bool getResolveAllFlags() const {
            return this->config.resolveAllFlags;
        }

        [[nodiscard]] ParseCallback<void> getErrorDisplay() const {
            return this->config.displayError;
        }

        [[nodiscard]] ParseCallback<T> getExecutable() const {
            return this->config.executable;
        }

        [[nodiscard]] std::vector<ParserParameter> getParams() const {
            return this->config.parameters;
        }

        [[nodiscard]] ParseCallback<void> getPrintHelp() const {
            return this->config.printHelp;
        }
    };
}