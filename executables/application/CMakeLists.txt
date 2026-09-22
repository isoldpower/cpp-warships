# Compile files
add_executable(cpp_warships main.cpp)

# Link dependencies
target_link_libraries(cpp_warships
        PRIVATE game_tui
        PRIVATE utilities
        PRIVATE cxx_setup
)
