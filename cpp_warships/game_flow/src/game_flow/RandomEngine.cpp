#include <game_flow/RandomEngine.h>

namespace cpp_warships::game_flow {

    RandomEngine makeRandomlySeededEngine() {
        std::random_device randomDevice;
        return RandomEngine{randomDevice()};
    }
} // namespace cpp_warships::game_flow
