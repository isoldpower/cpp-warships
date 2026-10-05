#include <utilities/TypesHelper.h>

#include <stdexcept>
#include <string>
#include <utility>

std::pair<int, int> TypesHelper::convertToPair(const std::string& input) {
    if (input.length() < 2) {
        throw std::invalid_argument("Input string is too short");
    }

    char letter = input[0];
    if (letter < 'A' || letter > 'Z') {
        throw std::invalid_argument("First character is not a capitalized English letter");
    }

    int letterValue = letter - 'A';
    int numberValue = std::stoi(input.substr(1)) - 1;

    return {letterValue, numberValue};
}

std::pair<int, int> TypesHelper::cell(const std::string& coordinate) {
    if (coordinate.length() < 2) {
        throw std::invalid_argument("Input string is too short");
    }

    char letter = coordinate[0];
    if (letter < 'A' || letter > 'Z') {
        throw std::invalid_argument("First character is not a capitalized English letter");
    }

    int letterValue = letter - 'A';
    int numberValue = std::stoi(coordinate.substr(1)) - 1;

    return {letterValue, numberValue};
}
