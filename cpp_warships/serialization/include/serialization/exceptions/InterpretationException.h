#pragma once

#include <exception>
#include <string>

namespace cpp_warships::game_saves::exceptions {

    /**
     * @brief Exception thrown when down-casting fails or when configured object cannot be interpreted correctly.
     */
    class InterpretationException : public std::exception {
    private:
        std::string message;
        std::string errorMessage;

    public:
        /**
         * @brief Constructs a SerializationException with a given message.
         * @param message The error message.
         */
        explicit InterpretationException(const std::string& message)
            : message(message)
            , errorMessage("InterpretationException: " + message)
        {}

        /**
         * @brief Returns the error message.
         * @return The error message.
         */
        [[nodiscard]] const char* what() const noexcept override {
            return errorMessage.c_str();
        }
    };

} // namespace cpp_warships::game_saves::exceptions