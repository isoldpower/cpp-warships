cmake_minimum_required(VERSION 3.20)
project (cpp_warships
        VERSION 0.2.0
        DESCRIPTION "C++ terminal-based warships game"
        LANGUAGES CXX)

if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Release CACHE STRING "Type of the build" FORCE)
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# Emit compile_commands.json so tooling resolves include paths without an IDE.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Tests build only in Release; WebAssembly is excluded since its binaries cannot run here
if (CMAKE_BUILD_TYPE STREQUAL "Release" AND NOT EMSCRIPTEN)
    set (BUILD_TESTING ON CACHE BOOL "Enable testing" FORCE)
    include(CTest)
    message(STATUS "Including tests into final build")
else()
    set (BUILD_TESTING OFF CACHE BOOL "Enable testing" FORCE)
    message(STATUS "Tests are not available in ${CMAKE_BUILD_TYPE} mode")
endif()

# Shared compiler settings; libraries expose their own include roots.
add_library(cxx_setup INTERFACE)

# General compiler options
if (MSVC)
    target_compile_options(cxx_setup INTERFACE /W4 /WX /permissive-)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(cxx_setup INTERFACE -Wall -Wextra -Wpedantic)
endif()

target_compile_features(cxx_setup
        INTERFACE cxx_std_20)

# WebAssembly: FTXUI blocks waiting for keys, so the game runs off the main thread,
# and shared memory must be on for every object in the binary, not only FTXUI's.
if (EMSCRIPTEN)
    string(APPEND CMAKE_CXX_FLAGS " -pthread")
    message(STATUS "Building for WebAssembly with thread support")
endif()

# Subdirectories
add_subdirectory(external)
add_subdirectory(${PROJECT_NAME})
add_subdirectory(executables)
