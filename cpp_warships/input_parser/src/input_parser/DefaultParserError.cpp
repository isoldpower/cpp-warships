#include <input_parser/DefaultParserError.h>

#include <utilities/ViewHelper.h>


namespace cpp_warships::input_parser {
    void DefaultParserError::CommandNotFoundError(ParsedOptions options) {
        ViewHelper::consoleOut("\nRelated command not found. Please, try again \n");
        ViewHelper::consoleOut("Parsed options list: \n");
        for (const auto &[key, value] : options) {
            ViewHelper::consoleOut(key + ": " + value, 1);
        }
    }

    void DefaultParserError::WrongFlagValueError(ParsedOptions options) {
        ViewHelper::consoleOut(
                "\nUnresolved combination of command and flag. \n "
                "It probably means that flag that you have entered either not listed in specification "
                "or \n"
                "the value you passed doesn't pass the validation tests \n");

        ViewHelper::consoleOut("Parsed options list: \n");
        for (const auto &[key, value] : options) {
            ViewHelper::consoleOut(key + ": " + value, 1);
        }
    }
}