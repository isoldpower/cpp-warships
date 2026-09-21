cmake_minimum_required(VERSION 3.20)
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

# Shared compiler settings carrier. Libraries expose their own include roots,
# so this target deliberately adds no include directories of its own.
add_library(cxx_setup INTERFACE)

# General compiler options
if (MSVC)
    target_compile_options(cxx_setup INTERFACE /W4 /WX /permissive-)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(cxx_setup INTERFACE -Wall -Wextra -Wpedantic)
endif()

target_compile_features(cxx_setup
        INTERFACE cxx_std_20)

# Subdirectories
add_subdirectory(external)
add_subdirectory(${PROJECT_NAME})
add_subdirectory(executables)
