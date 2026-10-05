#pragma once
#include <string>
#include <vector>

class StringHelper {
public:
    static std::vector<std::string> split(const std::string& text, char separator);
    static std::string patternCoordinate(int fieldSize);
    static std::string toLower(const std::string& text);
    static std::string trim(const std::string& text);
};
