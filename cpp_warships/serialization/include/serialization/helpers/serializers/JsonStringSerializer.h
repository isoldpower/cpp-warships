#pragma once

#include <string>

namespace cpp_warships::game_saves::helpers::serializers {
    /**
     * @brief JsonStringSerializer is a utility class that provides methods to extract field values from a serialized string representation.
     *
     * JsonStringSerializer is designed to work with serialized strings that represent objects in a JSON-like format.
     */
    class JsonStringSerializer {
    public:
        /**
         * @brief Extracts the value of a specified field from a serialized string.
         * @param data The serialized string containing the field.
         * @param fieldName The name of the field to extract.
         * @param isImplicit Indicates whether the field is an object that is serialized with this class. Defaults to false.
         * @return A pointer to a string containing the field value, or nullptr if the field is not found.
         */
        static std::string* extractFieldValue(
            std::string& data,
            const std::string& fieldName,
            bool isImplicit = false
        ) {
            size_t fieldLength = fieldName.length();
            size_t separatorLength = 2; // ": "
            size_t startPos = data.find(fieldName);
            size_t finalStartPos = startPos + fieldLength + separatorLength;

            if (startPos == std::string::npos) {
                return nullptr;
            } else {
                size_t endPos = data.find(isImplicit ? "};" : ";", finalStartPos);
                if (endPos == std::string::npos) {
                    // If no comma found, take the rest of the string
                    endPos = data.length();
                }

                return new std::string(data.substr(finalStartPos, endPos - finalStartPos));
            }
        }

        /**
         * @brief Sets the value for a specified field in a serialized string.
         * @param field A pointer to the field where the value will be set.
         * @param data The serialized string containing the field.
         * @param fieldName The name of the field to set.
         * @param converter A function to convert the extracted value (default is identity).
         * @return True if the field was found and set, false otherwise.
         *
         * Method is searching for a field in the serialized string and sets its value to the provided reference field.
         */
        template <typename T = std::string>
        static bool setFieldValue(
            T* field,
            std::string& data,
            const std::string& fieldName,
            std::function<T(std::string&)> converter = [](std::string& value) {
                return value;
            }
        ) {
            if (auto value = extractFieldValue(data, fieldName)) {
                *field = converter(*value);
                delete value;
                return true;
            }

            return false;
        }

        /**
         * @brief Checks if the serialized string contains all specified fields.
         * @param item The serialized string to check.
         * @param fields The fields to check for.
         * @return True if all specified fields are present, false otherwise.
         *
         * Method checks if the serialized string contains all specified fields in the format "fieldName: value".
         */
        template <typename ...Fields>
        static bool isIncludeFields(const std::string &item, Fields... fields) {
            for (const std::string& field : {fields...}) {
                std::string expectedPrefix = field + ": ";

                if (item.find(expectedPrefix) == std::string::npos) {
                    return false;
                }
            }

            return true;
        }

        /**
         * @brief Checks if the string is an item serialized in a class format.
         * @param item The string to check.
         * @return True if an argument is a related item, false otherwise.
         *
         * Method checks if the item contains curly braces and a colon, indicating it is in a correct format.
         */
        static bool isRelatedItem(const std::string& item) {
            return item.find('{') != std::string::npos &&
                   item.find('}') != std::string::npos &&
                   item.find(':') != std::string::npos;
        }

        /**
         * @brief Serializes a map of fields into a string representation.
         * @param fields Reference to the map of fields to serialize.
         * @return A string representation of the serialized fields in a JSON-like format.
         *
         * Method serializes a map of fields into a string format, where each field is represented as "title: value".
         * All fields include a semicolon at the end, including related items.
         */
        static std::string serializeFields(const std::unordered_map<std::string, std::string>& fields) {
            std::string result;
            for (const auto& [title, value] : fields) {
                std::string fieldRepresentation = std::string(title)
                    .append(": ")
                    .append(value);
                result += fieldRepresentation;

                // Related items already include a semicolon and nextline symbol at the end
                if (!isRelatedItem(fieldRepresentation)) {
                    result += ";\n";
                }
            }

            return "{\n" + result + "};\n";
        }
    };
} // namespace cpp_warships::game_saves::helpers::serializers