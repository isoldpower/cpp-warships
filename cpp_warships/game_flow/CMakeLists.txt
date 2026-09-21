# Compile files
add_library(game_flow
        # Vocabulary
        include/game_flow/Participant.h
        include/game_flow/SkillKind.h
        include/game_flow/MatchEvent.h
        include/game_flow/AttackOutcomeBehaviour.h src/game_flow/AttackOutcomeBehaviour.cpp
        include/game_flow/RandomEngine.h src/game_flow/RandomEngine.cpp
        # Match parts
        include/game_flow/EventLog.h src/game_flow/EventLog.cpp
        include/game_flow/TurnOrder.h src/game_flow/TurnOrder.cpp
        include/game_flow/ShotStrength.h src/game_flow/ShotStrength.cpp
        include/game_flow/SkillQueue.h src/game_flow/SkillQueue.cpp
        include/game_flow/SkillManager.h src/game_flow/SkillManager.cpp
        include/game_flow/SkillBehaviour.h src/game_flow/SkillBehaviour.cpp
        include/game_flow/PlacementPlan.h src/game_flow/PlacementPlan.cpp
        include/game_flow/AiOpponent.h src/game_flow/AiOpponent.cpp
        include/game_flow/Match.h src/game_flow/Match.cpp
)

# Tests setup
if (BUILD_TESTING)
    add_executable(game_flow_tests tests/main.cpp)
    target_link_libraries(game_flow_tests PRIVATE GTest::gtest_main game_flow)
    include(GoogleTest)
    gtest_discover_tests(game_flow_tests)
endif()

# Expose settings
target_include_directories(game_flow
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link dependencies. game_flow orchestrates the rules and, like game_core,
# performs no input or output of its own.
target_link_libraries(game_flow PUBLIC game_core cxx_setup)
