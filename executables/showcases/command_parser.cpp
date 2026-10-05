#include <input_parser/CommandParser.h>
#include <input_parser/VoidParser.h>
#include <utilities/ViewHelper.h>

#include "DemoScheme.h"

using namespace cpp_warships::input_parser;
using namespace cpp_warships::input_parser::builder;
using namespace cpp_warships::input_parser::command;
using namespace cpp_warships::input_parser::model;

class ExitCommand : public ParserCommand {
    void execute(ParsedOptions) override {
        ViewHelper::consoleOut("Leaving");
    }
};

class NewCommand : public ParserCommand {
    void execute(ParsedOptions) override {
        ViewHelper::consoleOut("New");
    }
};

class InfoCommand : public ParserCommand {
    void execute(ParsedOptions) override {
        ViewHelper::consoleOut("Info");
    }
};

class ListCommand : public ParserCommand {
    void execute(ParsedOptions) override {
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
    ParserCommand* handleExit(ParsedOptions) {
        isExited = true;
        return new ExitCommand();
    }

    ParserCommand* handleNew(ParsedOptions) {
        return new NewCommand();
    }

    ParserCommand* handleInfo(ParsedOptions) {
        return new InfoCommand();
    }

    ParserCommand* handleList(ParsedOptions) {
        return new ListCommand();
    }

    ParserCommand* handleLoad(ParsedOptions) {
        return new LoadCommand();
    }

    void displayError(const ParsedOptions& options) {
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
    ParserCommandsHandler handler;
    const auto displayError = [&handler](const ParsedOptions& options) {
        handler.displayError(options);
    };
    CommandParser parser(
        cpp_warships::showcases::demoScheme<ParserCommand*>(handler),
        displayError
    );

    while (!handler.getExited()) {
        std::cout << "Enter new command (help for list of commands): ";
        std::string input;
        std::getline(std::cin, input);
        parser.executedParse(input);
    }
}
