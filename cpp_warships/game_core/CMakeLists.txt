# Compile files
add_library(game_core
        # Value types
        include/game_core/Coordinate.h
        include/game_core/Direction.h
        include/game_core/Outcomes.h
        include/game_core/Segment.h src/game_core/Segment.cpp
        # Entities
        include/game_core/Ship.h src/game_core/Ship.cpp
        include/game_core/Board.h src/game_core/Board.cpp
        # Rules configuration
        include/game_core/FleetComposition.h src/game_core/FleetComposition.cpp
        include/game_core/MatchSettings.h src/game_core/MatchSettings.cpp
)

# Tests setup
if (BUILD_TESTING)
    add_executable(game_core_tests tests/main.cpp)
    target_link_libraries(game_core_tests PRIVATE GTest::gtest_main game_core)
    include(GoogleTest)
    gtest_discover_tests(game_core_tests)
endif()

# Expose settings
target_include_directories(game_core
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link dependencies. game_core is pure rules: it deliberately depends on no
# project library, so it stays free of I/O and of any UI toolkit.
target_link_libraries(game_core PUBLIC cxx_setup)
