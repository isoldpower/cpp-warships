#pragma once
#include <vector>


using FieldCoordinate = std::pair<int, int>;

class FieldCoordinateHelper {
public:
    static int findMaxPairValue(const std::vector<FieldCoordinate>& vec, bool first);
    static int findMinPairValue(const std::vector<FieldCoordinate>& vec, bool first);
};
