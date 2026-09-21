#include <input_parser/command/ErrorCommand.h>


namespace cpp_warships::input_parser {
    ErrorCommand::ErrorCommand(std::function<void(ParsedOptions options)> displayError)
        : ParserCommand()
        , displayError(std::move(displayError))
    {}

    void ErrorCommand::execute(ParsedOptions options) {
        displayError(options);
    }
}