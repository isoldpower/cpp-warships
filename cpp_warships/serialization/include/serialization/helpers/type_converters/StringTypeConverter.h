#pragma once

#include <iostream>
#include <stdexcept>
#include <string>

namespace cpp_warships::serialization::helpers::type_converters {
    /** @brief StringTypeConverter is a utility class that provides methods to
     * convert from string to various types and vice versa. */
    class StringTypeConverter {
    public:
        /** @brief Converts a string to an integer value. */
        static int stringToInt(const std::string& data) {
            try {
                return std::stoi(data);
            } catch (const std::exception& error) {
                std::cerr << error.what() << std::endl;
                throw std::runtime_error("Failed to convert string to int");
            }
        }

        /** @brief Converts a string to a float value. */
        static float stringToFloat(const std::string& data) {
            try {
                return std::stof(data);
            } catch (const std::exception& error) {
                std::cerr << error.what() << std::endl;
                throw std::runtime_error("Failed to convert string to float");
            }
        }
    };
}  // namespace cpp_warships::serialization::helpers::type_converters