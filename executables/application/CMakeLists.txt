# Compile files
add_executable(cpp_warships main.cpp)

# Link dependencies
target_link_libraries(cpp_warships
        PRIVATE ftxui::ftxui
        PRIVATE utilities
        PRIVATE cxx_setup
)
