#pragma once

#include <exception>
#include <string>

namespace cpp_warships::serialization::exceptions {
    /** @brief Exception thrown when serialization of a specific object fails. */
    class SerializationException : public std::exception {
    private:
        std::string passedMessage_;
        std::string objectType_;
        std::string constructedMessage_;

    public:
        /** @brief Constructs a SerializationException with a given message. */
        explicit SerializationException(const std::string& type, const std::string& message)
            : passedMessage_(message)
            , objectType_(type)
            , constructedMessage_('\n' + objectType_ + " serialization error: " + message) {}

        /** @brief Returns the error message. @return The error message. */
        [[nodiscard]] const char* what() const noexcept override {
            return constructedMessage_.c_str();
        }
    };
}  // namespace cpp_warships::serialization::exceptions