#include <utilities/StringHelper.h>

#include <algorithm>
#include <stdexcept>


std::vector<std::string> StringHelper::split(const std::string& initial, char delim) {
    std::vector<std::string> elems;
    std::string current;

    for (int i = 0; i < static_cast<int>(initial.length()); i++) {
        if (initial[i] == delim) {
            elems.push_back(current);
            current = "";
        } else {
            current += initial[i];
        }
    }

    if (!current.empty()) {
        elems.push_back(current);
    }

    return elems;
}

std::string StringHelper::patternCoordinate(int fieldSize) {
    std::string pattern;
    if (fieldSize < 10 || fieldSize > 26) {
        throw std::invalid_argument("Method functions support field size from 10 to 25");
    }
    if (fieldSize < 20) {
        pattern = "^([0-9]|1[0-" + std::to_string(fieldSize % 10) + "])\\,([0-9]|1[0-" +
                  std::to_string(fieldSize % 10) + "])$";
    } else if (fieldSize - 1 < 26) {
        pattern = "^([0-9]|1[0-9]|2[0-" + std::to_string(fieldSize % 10) +
                  "])\\,([0-9]|1[0-9]|2[0-" + std::to_string(fieldSize % 10) + "])$";
    }

    return pattern;
}

std::string StringHelper::toLower(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string StringHelper::trim(const std::string& str) {
    auto start = str.find_first_not_of(' ');
    auto end = str.find_last_not_of(' ');

    if (start == std::string::npos || end == std::string::npos) {
        return "";
    }

    return str.substr(start, end - start + 1);
}
