#include <utilities/FieldCoordinateHelper.h>

#include <algorithm>


int FieldCoordinateHelper::findMaxPairValue(const std::vector<FieldCoordinate>& vec, bool first) {
    auto maxElement = std::max_element(
            vec.begin(), vec.end(), [first](const FieldCoordinate& a, const FieldCoordinate& b) {
                return first ? (a.first < b.first) : (a.second < b.second);
            });

    return maxElement != vec.end() ? first ? maxElement->first : maxElement->second : -1;
}

int FieldCoordinateHelper::findMinPairValue(const std::vector<FieldCoordinate>& vec, bool first) {
    auto minElement = std::min_element(
            vec.begin(), vec.end(), [first](const FieldCoordinate& a, const FieldCoordinate& b) {
                return first ? (a.first < b.first) : (a.second < b.second);
            });

    return minElement != vec.end() ? first ? minElement->first : minElement->second : -1;
}
