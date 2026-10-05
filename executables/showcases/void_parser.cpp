#include <input_parser/CommandParser.h>
#include <input_parser/VoidParser.h>
#include <utilities/ViewHelper.h>

#include "DemoScheme.h"

using namespace cpp_warships::input_parser;
using namespace cpp_warships::input_parser::builder;
using namespace cpp_warships::input_parser::command;
using namespace cpp_warships::input_parser::model;

class ParserCommandsHandler {
private:
    bool isExited = false;

public:
    void handleExit(ParsedOptions) {
        ViewHelper::consoleOut("Leaving");
        isExited = true;
    }

    void handleNew(ParsedOptions) {
        ViewHelper::consoleOut("New");
    }

    void handleInfo(ParsedOptions) {
        ViewHelper::consoleOut("Info");
    }

    void handleList(ParsedOptions) {
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
    ParserCommandsHandler handler;
    const SchemeMap<void> inputScheme = cpp_warships::showcases::demoScheme<void>(handler);

    while (!handler.getExited()) {
        std::cout << "Enter new command (help for list of commands): ";
        std::string input;
        std::getline(std::cin, input);
        VoidParser parser(inputScheme);
        parser.executedParse(input);
    }
}
