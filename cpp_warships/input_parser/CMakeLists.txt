# Compile files
add_library(input_parser
        # Parser
        include/input_parser/command/Command.h
        include/input_parser/model/Parser.h
        include/input_parser/command/ParserCommand.h
        include/input_parser/model/ParserCommandInfo.h
        include/input_parser/model/ParserParameter.h
        # Void Parser Implementation
        include/input_parser/VoidParser.h src/input_parser/VoidParser.cpp
        include/input_parser/DefaultHelp.h src/input_parser/DefaultHelp.cpp
        include/input_parser/DefaultParserError.h src/input_parser/DefaultParserError.cpp
        # Command Parser Implementation
        include/input_parser/CommandParser.h src/input_parser/CommandParser.cpp
        include/input_parser/command/ErrorCommand.h src/input_parser/command/ErrorCommand.cpp
        include/input_parser/command/HelpCommand.h src/input_parser/command/HelpCommand.cpp
        include/input_parser/command/ArgumentsErrorCommand.h src/input_parser/command/ArgumentsErrorCommand.cpp
        # Builder
        include/input_parser/builder/ConfigCommandBuilder.h
        include/input_parser/builder/ParserCommandBuilder.h
        include/input_parser/builder/DefaultParameterBuilder.h
        include/input_parser/builder/ParserParameterBuilder.h
        include/input_parser/builder/CommandBuildDirector.h
        include/input_parser/builder/ParameterBuildDirector.h
        include/input_parser/builder/ParserBuilderDirector.h)

# Tests setup
if (BUILD_TESTING)
    add_executable(input_parser_tests tests/main.cpp)
    target_link_libraries(input_parser_tests PRIVATE GTest::gtest_main input_parser)
    include(GoogleTest)
    gtest_discover_tests(input_parser_tests)
endif()

# Expose settings
target_include_directories(input_parser
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link dependencies
target_link_libraries(input_parser PUBLIC utilities cxx_setup)
