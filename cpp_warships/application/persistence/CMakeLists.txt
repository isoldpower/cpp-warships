# Compile files
add_library(warships_persistence
        # Where saves are kept
        include/application/persistence/SaveStorage.h
        include/application/persistence/SaveSummary.h
        include/application/persistence/FilesystemSaveStorage.h src/application/persistence/FilesystemSaveStorage.cpp
        include/application/persistence/MemorySaveStorage.h src/application/persistence/MemorySaveStorage.cpp
        # What is saved
        include/application/persistence/MatchSnapshot.h src/application/persistence/MatchSnapshot.cpp
        include/application/persistence/SaveArchive.h src/application/persistence/SaveArchive.cpp
        # How it is written
        include/application/persistence/serializers/SegmentJsonSerializer.h src/application/persistence/serializers/SegmentJsonSerializer.cpp
        include/application/persistence/serializers/ShipJsonSerializer.h src/application/persistence/serializers/ShipJsonSerializer.cpp
        include/application/persistence/serializers/BoardJsonSerializer.h src/application/persistence/serializers/BoardJsonSerializer.cpp
        include/application/persistence/serializers/MatchSettingsJsonSerializer.h src/application/persistence/serializers/MatchSettingsJsonSerializer.cpp
        include/application/persistence/serializers/MatchSnapshotJsonSerializer.h src/application/persistence/serializers/MatchSnapshotJsonSerializer.cpp
)

# Local storage only exists in a browser, so only a browser build carries it.
if (EMSCRIPTEN)
    target_sources(warships_persistence PRIVATE
            include/application/persistence/BrowserSaveStorage.h
            src/application/persistence/BrowserSaveStorage.cpp)
    target_link_libraries(warships_persistence PUBLIC browser_save_storage_binding)
endif()

# Tests setup
if (BUILD_TESTING)
    add_executable(warships_persistence_tests
            tests/FilesystemSaveStorageTests.cpp
            tests/JsonSerializerTests.cpp
            tests/MatchSnapshotTests.cpp
            tests/MemorySaveStorageTests.cpp
            tests/SaveArchiveTests.cpp)
    target_link_libraries(warships_persistence_tests PRIVATE GTest::gtest_main warships_persistence)
    include(GoogleTest)
    gtest_discover_tests(warships_persistence_tests)
endif()

# Expose settings
target_include_directories(warships_persistence
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link dependencies
target_link_libraries(warships_persistence
        # nlohmann/json is in the serializers' public headers, so it travels with them
        PUBLIC warships_core warships_flow serialization cxx_setup nlohmann_json::nlohmann_json
        PRIVATE platform
)
