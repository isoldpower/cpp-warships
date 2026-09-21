# Compile files
add_library(input_reader
        include/input_reader/InputReader.h
        include/input_reader/command_reader/CommandInputReader.h src/input_reader/command_reader/CommandInputReader.cpp
        include/input_reader/config_reader/ConfigInputReader.h src/input_reader/config_reader/ConfigInputReader.cpp
        include/input_reader/console_reader/ConsoleInputReader.h src/input_reader/console_reader/ConsoleInputReader.cpp
)

# Tests setup
if (BUILD_TESTING)
    add_executable(input_reader_tests tests/main.cpp)
    target_link_libraries(input_reader_tests PRIVATE GTest::gtest_main input_reader)
    include(GoogleTest)
    gtest_discover_tests(input_reader_tests)
endif()

# Expose settings
target_include_directories(input_reader
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link dependencies
target_link_libraries(input_reader PUBLIC utilities cxx_setup)
