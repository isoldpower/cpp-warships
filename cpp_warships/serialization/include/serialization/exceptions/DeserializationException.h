#pragma once

#include <exception>
#include <string>

namespace cpp_warships::game_saves::exceptions {
    /**
     * @brief Exception thrown when deserialization of a specific object fails.
     */
    class DeserializationException : public std::exception {
    private:
        std::string message;
        std::string objectType;
        std::string errorMessage;

    public:
        /**
         * @brief Constructs a DeserializationException with a given message.
         * @param type The type of the object that failed to deserialize.
         * @param message The error message.
         */
        explicit DeserializationException(const std::string& type, const std::string& message)
            : message(message)
            , objectType(type)
            , errorMessage(objectType + " deserialization error: " + message)
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