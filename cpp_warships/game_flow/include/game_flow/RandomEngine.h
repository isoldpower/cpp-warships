#pragma once

#include <random>

namespace cpp_warships::game_flow {
    /** @brief The random source the whole flow layer draws from, passed by reference.
     *  Building one per call would cost a match its reproducibility from a single seed. */
    using RandomEngine = std::mt19937;

    /** @brief A randomly seeded engine, for when reproducibility is not required. */
    RandomEngine makeRandomlySeededEngine();
} // namespace cpp_warships::game_flow
