#pragma once

#include <input_parser/builder/ConfigCommandBuilder.h>
#include <input_parser/builder/DefaultParameterBuilder.h>
#include <input_parser/model/ParserCommandInfo.h>

#include <regex>
#include <string>
#include <vector>

namespace cpp_warships::showcases {
    using input_parser::model::ParsedOptions;
    using input_parser::model::ParserCommandInfo;
    using input_parser::model::ParserParameter;
    using input_parser::model::SchemeMap;

    /** @brief A flag a demo command takes: what it is, what it may hold, whether it must be given.
     */
    inline ParserParameter parameter(
        const std::string& flag,
        const std::string& description,
        const std::string& validatorPattern,
        const bool isNecessary
    ) {
        input_parser::builder::DefaultParameterBuilder builder;
        return builder.addFlag(flag)
            .setDescription(description)
            .setValidator(std::regex(validatorPattern))
            .setNecessary(isNecessary)
            .buildAndReset();
    }

    /** @brief A demo command that calls @p method on @p handler with whatever was parsed. */
    template <typename Result, typename Handler>
    ParserCommandInfo<Result> commandInfo(
        Handler& handler,
        Result (Handler::*method)(ParsedOptions),
        const std::string& description,
        const std::vector<ParserParameter>& parameters = {}
    ) {
        input_parser::builder::ConfigCommandBuilder<Result> builder;
        builder.setCallback([&handler, method](ParsedOptions options) {
            return (handler.*method)(options);
        });
        builder.setDescription(description);
        for (const ParserParameter& flag : parameters) {
            builder.addParameter(flag);
        }

        return ParserCommandInfo<Result>(builder.buildAndReset());
    }

    template <typename Result, typename Handler>
    ParserCommandInfo<Result> loadCommand(Handler& handler) {
        return commandInfo<Result>(
            handler,
            &Handler::handleLoad,
            "Load game from file",
            {parameter(
                "--filename",
                "Specify the file name to load the game from. Make sure it's a .json file",
                "^.*\\.json$",
                true
            )}
        );
    }

    template <typename Result, typename Handler>
    ParserCommandInfo<Result> newCommand(Handler& handler) {
        return commandInfo<Result>(
            handler,
            &Handler::handleNew,
            "Start new game from scratch",
            {parameter(
                "--default",
                "Start game with default settings and skip the initialization phase (true/false",
                "^(true|false)$",
                false
            )}
        );
    }

    template <typename Result, typename Handler>
    ParserCommandInfo<Result> listCommand(Handler& handler) {
        return commandInfo<Result>(
            handler,
            &Handler::handleList,
            "List all available saves",
            {parameter("--filename", "Specify path to the directory with saves", "^.*$", false)}
        );
    }

    /** @brief The commands both parser demos answer to, each calling its handler on @p handler. */
    template <typename Result, typename Handler>
    SchemeMap<Result> demoScheme(Handler& handler) {
        return {
            {"load", loadCommand<Result>(handler)},
            {"new", newCommand<Result>(handler)},
            {"info",
             commandInfo<Result>(
                 handler,
                 &Handler::handleInfo,
                 "Print the latest screenshot from currently handled save"
             )},
            {"list", listCommand<Result>(handler)},
            {"exit", commandInfo<Result>(handler, &Handler::handleExit, "Exit the program")},
        };
    }
}  // namespace cpp_warships::showcases
