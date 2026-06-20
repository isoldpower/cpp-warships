cmake_minimum_required(VERSION 3.5...3.31)
project (cpp_warships
        VERSION 0.1.0
        DESCRIPTION "C++ terminal-based warships game"
        LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Release CACHE STRING "Type of the build" FORCE)
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# Enable testing depending on the build type
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    include(CTest)
    message(STATUS "Including tests into final build")
else()
    set (BUILD_TESTING OFF CACHE BOOL "Enable testing" FORCE)
    message(STATUS "Tests are not available in Release mode")
endif()

# Libraries list
add_library(cxx_setup INTERFACE
        cpp_warships/utilities
        cpp_warships/input_parser
        cpp_warships/input_reader
        cpp_warships/game_saves)

# General compiler options
if (MSVC)
    target_compile_options(cxx_setup INTERFACE /W4 /WX /permissive-)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(cxx_setup INTERFACE -Wall -Wextra -Wpedantic)
endif()

target_compile_features(cxx_setup
        INTERFACE cxx_std_20)
target_include_directories(cxx_setup
        INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Subdirectories
add_subdirectory(external)
add_subdirectory(${PROJECT_NAME})
add_subdirectory(executables)