#include <input_parser/command/ArgumentsErrorCommand.h>

namespace cpp_warships::input_parser {
    void ArgumentsErrorCommand::execute(ParsedOptions options) {
        ParseCallback<void> protectedDisplayError = command.getErrorDisplay()
            ? command.getErrorDisplay()
            : throw std::invalid_argument("Arguments validation failed. You can get better error message by providing displayError callback");

        protectedDisplayError(options);
    }
}