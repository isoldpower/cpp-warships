#include <input_parser/CommandParser.h>
#include <utilities/ViewHelper.h>
#include <input_parser/VoidParser.h>
#include <input_parser/builder/ConfigCommandBuilder.h>
#include <input_parser/builder/DefaultParameterBuilder.h>

using namespace cpp_warships::input_parser;

class ParserCommandsHandler {
private:
    bool isExited = false;
public:
    void handleExit(ParsedOptions options) {
        ViewHelper::consoleOut("Leaving");
        isExited = true;
    }

    void handleNew(ParsedOptions options) {
        ViewHelper::consoleOut("New");
    }

    void handleInfo(ParsedOptions options) {
        ViewHelper::consoleOut("Info");
    }

    void handleList(ParsedOptions options) {
        ViewHelper::consoleOut("List");
    }

    void handleLoad(ParsedOptions options) {
        ViewHelper::consoleOut("Load from " + options["filename"]);
    }

    bool getExited() {
        return isExited;
    }
};

int main() {
    ConfigCommandBuilder<void> commandBuilder;
    DefaultParameterBuilder parameterBuilder;
    ParserCommandsHandler handler;

    SchemeMap<void> inputScheme = {
        {"load", ParserCommandInfo(
            commandBuilder
                .setCallback(TypesHelper::methodToFunction(&ParserCommandsHandler::handleLoad, &handler))
                .setDescription("Load game from file")
                .addParameter(
                    parameterBuilder
                        .addFlag("--filename")
                        .setDescription("Specify the file name to load the game from. Make sure it's a .json file")
                        .setValidator(std::regex("^.*\\.json$"))
                        .setNecessary(true)
                        .buildAndReset()
                )
                .buildAndReset()
            )
        },
        {"new", ParserCommandInfo(
            commandBuilder
                .setCallback(TypesHelper::methodToFunction(&ParserCommandsHandler::handleNew, &handler))
                .setDescription("Start new game from scratch")
                .addParameter(
                    parameterBuilder
                        .addFlag("--default")
                        .setDescription("Start game with default settings and skip the initialization phase (true/false")
                        .setNecessary(false)
                        .setValidator(std::regex("^(true|false)$"))
                        .buildAndReset()
                    )
                .buildAndReset()
            )
        },
        {"info", ParserCommandInfo(
            commandBuilder
                .setCallback(TypesHelper::methodToFunction(&ParserCommandsHandler::handleInfo, &handler))
                .setDescription("Print the latest screenshot from currently handled save")
                .buildAndReset()
            )
        },
        {"list", ParserCommandInfo(
            commandBuilder
                .setCallback(TypesHelper::methodToFunction(&ParserCommandsHandler::handleList, &handler))
                .setDescription("List all available saves")
                .addParameter(
                    parameterBuilder
                        .addFlag("--filename")
                        .setNecessary(false)
                        .setDescription("Specify path to the directory with saves")
                        .setValidator(std::regex("^.*$"))
                        .buildAndReset()
                )
                .buildAndReset()
            )
        },
        {"exit", ParserCommandInfo(
            commandBuilder
                .setCallback(TypesHelper::methodToFunction(&ParserCommandsHandler::handleExit, &handler))
                .setDescription("Exit the program")
                .buildAndReset()
            )
        }
    };

    while (!handler.getExited()) {
        std::cout << "Enter new command (help for list of commands): ";
        std::string input;
        std::getline(std::cin, input);
        VoidParser parser(inputScheme);
        parser.executedParse(input);
    }
}
