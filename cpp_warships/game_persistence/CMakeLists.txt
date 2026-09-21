# Compile files
add_library(game_persistence
        # Where saves are kept
        include/game_persistence/SaveStorage.h
        include/game_persistence/FilesystemSaveStorage.h src/game_persistence/FilesystemSaveStorage.cpp
        include/game_persistence/MemorySaveStorage.h src/game_persistence/MemorySaveStorage.cpp
        # What is saved
        include/game_persistence/MatchSnapshot.h src/game_persistence/MatchSnapshot.cpp
        include/game_persistence/SaveArchive.h src/game_persistence/SaveArchive.cpp
        # How it is written
        include/game_persistence/serializers/SegmentJsonSerializer.h src/game_persistence/serializers/SegmentJsonSerializer.cpp
        include/game_persistence/serializers/ShipJsonSerializer.h src/game_persistence/serializers/ShipJsonSerializer.cpp
        include/game_persistence/serializers/BoardJsonSerializer.h src/game_persistence/serializers/BoardJsonSerializer.cpp
        include/game_persistence/serializers/MatchSettingsJsonSerializer.h src/game_persistence/serializers/MatchSettingsJsonSerializer.cpp
        include/game_persistence/serializers/MatchSnapshotJsonSerializer.h src/game_persistence/serializers/MatchSnapshotJsonSerializer.cpp
)

# Tests setup
if (BUILD_TESTING)
    add_executable(game_persistence_tests tests/main.cpp)
    target_link_libraries(game_persistence_tests PRIVATE GTest::gtest_main game_persistence)
    include(GoogleTest)
    gtest_discover_tests(game_persistence_tests)
endif()

# Expose settings
target_include_directories(game_persistence
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link dependencies
target_link_libraries(game_persistence
        PUBLIC game_core game_flow serialization cxx_setup
        PRIVATE nlohmann_json::nlohmann_json
)
