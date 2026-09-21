#pragma once

#include <string>

namespace cpp_warships::game_saves::helpers::type_converters {
    /**
     * @brief StringTypeConverter is a utility class that provides methods to convert from string to various types and vice versa.
     */
    class StringTypeConverter
    {
    public:
        /**
         * @brief Converts a string to an integer value.
         * @param data The string to convert.
         * @return Converted integer value or throws an `std::runtime_exception` if conversion fails.
         */
        static int stringToInt(const std::string& data) {
            try {
                return std::stoi(data);
            } catch (const std::exception& e) {
                std::cerr << e.what() << std::endl;
                throw std::runtime_error("Failed to convert string to int");
            }
        }

        /**
         * @brief Converts a string to a float value.
         * @param data The string to convert.
         * @return Converted float value or throws an `std::runtime_exception` if conversion fails.
         */
        static float stringToFloat(const std::string& data) {
            try {
                return std::stof(data);
            } catch (const std::exception& e) {
                std::cerr << e.what() << std::endl;
                throw std::runtime_error("Failed to convert string to float");
            }
        }
    };
} // namespace cpp_warships::game_saves::helpers