#pragma once

#include <string>

namespace cpp_warships::input_reader {
    template <typename T = std::string>
    class InputReader {
    public:
        virtual ~InputReader() = default;

        virtual T readCommand() = 0;
    };
} // namespace cpp_warships::input_reader
