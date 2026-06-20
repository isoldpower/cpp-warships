#include <cpp_warships/input_parser/include/CommandParser.h>
#include <cpp_warships/utilities/include/ViewHelper.h>
#include <cpp_warships/input_parser/include/VoidParser.h>
#include <cpp_warships/input_parser/include/builder/ConfigCommandBuilder.h>
#include <cpp_warships/input_parser/include/builder/DefaultParameterBuilder.h>

using namespace cpp_warships::input_parser;

class ExitCommand : public ParserCommand {
    void execute(ParsedOptions data) override {
        ViewHelper::consoleOut("Leaving");
    }
};

class NewCommand : public ParserCommand {
    void execute(ParsedOptions data) override {
        ViewHelper::consoleOut("New");
    }
};

class InfoCommand : public ParserCommand {
    void execute(ParsedOptions data) override {
        ViewHelper::consoleOut("Info");
    }
};

class ListCommand : public ParserCommand {
    void execute(ParsedOptions data) override {
        ViewHelper::consoleOut("List");
    }
};

class LoadCommand : public ParserCommand {
    void execute(ParsedOptions data) override {
        ViewHelper::consoleOut("Load from " + data["filename"]);
    }
};

class ParserCommandsHandler {
private:
    bool isExited = false;
public:
    ParserCommand* handleExit(ParsedOptions options) {
        isExited = true;
        return new ExitCommand();
    }

    ParserCommand* handleNew(ParsedOptions options) {
        return new NewCommand();
    }

    ParserCommand* handleInfo(ParsedOptions options) {
        return new InfoCommand();
    }

    ParserCommand* handleList(ParsedOptions options) {
        return new ListCommand();
    }

    ParserCommand* handleLoad(ParsedOptions options) {
        return new LoadCommand();
    }

    void displayError(const ParsedOptions &options) {
        ViewHelper::consoleOut("Bad input caused an error. Parsed options list:");
        for (const auto& option : options) {
            ViewHelper::consoleOut(option.first + ": " + option.second, 1);
        }
    };

    bool getExited() {
        return isExited;
    }
};

int main() {
    ConfigCommandBuilder<ParserCommand*> commandBuilder;
    DefaultParameterBuilder parameterBuilder;
    ParserCommandsHandler handler;

    SchemeMap<ParserCommand*> inputScheme = {
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

    auto errorHandler = TypesHelper::methodToFunction(&ParserCommandsHandler::displayError, &handler);
    CommandParser parser(inputScheme, errorHandler);

    while (!handler.getExited()) {
        std::cout << "Enter new command (help for list of commands): ";
        std::string input;
        std::getline(std::cin, input);
        parser.executedParse(input);
    }
}
