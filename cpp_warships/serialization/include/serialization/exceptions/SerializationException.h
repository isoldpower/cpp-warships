#pragma once

#include <exception>
#include <string>

namespace cpp_warships::game_saves::exceptions {

    /**
     * @brief Exception thrown when serialization of a specific object fails.
     */
    class SerializationException : public std::exception {
    private:
        std::string passed_message;
        std::string object_type;
        std::string constructed_message;

    public:
        /**
         * @brief Constructs a SerializationException with a given message.
         * @param type The type of the object that failed to serialize.
         * @param msg The error message.
         */
        explicit SerializationException(const std::string& type, const std::string& msg)
            : passed_message(msg)
            , object_type(type)
            , constructed_message('\n' + object_type + " serialization error: " + msg)
        {}

        /**
         * @brief Returns the error message.
         * @return The error message.
         */
        [[nodiscard]] const char* what() const noexcept override {
            return constructed_message.c_str();
        }
    };

} // namespace cpp_warships::game_saves::exceptions