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

namespace {
    constexpr int SMALLEST_DESCRIBED_FIELD = 10;
    constexpr int LARGEST_DESCRIBED_FIELD = 26;
    constexpr int DIGITS_PER_TEN = 10;

    /** @brief A pattern for one coordinate on a field @p fieldSize wide: any single digit, every
     * full ten below the field's own, then that ten up to the field's last digit. */
    std::string numberPattern(const int fieldSize) {
        const int highestTen = fieldSize / DIGITS_PER_TEN;
        std::string pattern = "([0-9]";

        for (int ten = 1; ten < highestTen; ++ten) {
            pattern += "|" + std::to_string(ten) + "[0-9]";
        }
        pattern += "|" + std::to_string(highestTen) + "[0-" +
                   std::to_string(fieldSize % DIGITS_PER_TEN) + "])";

        return pattern;
    }
}  // namespace

std::string StringHelper::patternCoordinate(int fieldSize) {
    if (fieldSize < SMALLEST_DESCRIBED_FIELD || fieldSize > LARGEST_DESCRIBED_FIELD) {
        throw std::invalid_argument("Method functions support field size from 10 to 26");
    }

    const std::string number = numberPattern(fieldSize);
    return "^" + number + "\\," + number + "$";
}

std::string StringHelper::toLower(const std::string& input) {
    std::string result = input;
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
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
