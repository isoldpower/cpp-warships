#include <utilities/TypesHelper.h>


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

std::pair<int, int> TypesHelper::cell(const std::string& coord) {
    if (coord.length() < 2) {
        throw std::invalid_argument("Input string is too short");
    }

    char letter = coord[0];
    if (letter < 'A' || letter > 'Z') {
        throw std::invalid_argument("First character is not a capitalized English letter");
    }

    int letterValue = letter - 'A';
    int numberValue = std::stoi(coord.substr(1)) - 1;

    return {letterValue, numberValue};
}
