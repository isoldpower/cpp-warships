# Compile files
add_library(utilities
        # Settings
        include/utilities/Initials.h
        # Helpers
        src/utilities/StateMessages.cpp include/utilities/StateMessages.h
        src/utilities/FieldCoordinateHelper.cpp include/utilities/FieldCoordinateHelper.h
        src/utilities/StringHelper.cpp include/utilities/StringHelper.h
        src/utilities/TypesHelper.cpp include/utilities/TypesHelper.h
        src/utilities/ViewHelper.cpp include/utilities/ViewHelper.h)

# Tests setup
if (BUILD_TESTING)
    add_executable(utilities_tests tests/main.cpp)
    target_link_libraries(utilities_tests PRIVATE GTest::gtest_main utilities)
    include(GoogleTest)
    gtest_discover_tests(utilities_tests)
endif()

# Expose settings
target_include_directories(utilities
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link dependencies
target_link_libraries(utilities PUBLIC cxx_setup)
