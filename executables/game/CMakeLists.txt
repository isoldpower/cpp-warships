# Compile files
add_executable(cpp_warships
        # Putting the parts together is this layer's whole job
        include/build_save_archive.h
        include/build_game.h src/build_game.cpp
        include/build_queries.h src/build_queries.cpp
        include/build_event_pipeline.h src/build_event_pipeline.cpp
        include/build_shell.h src/build_shell.cpp
        include/build_renderers.h src/build_renderers.cpp
        include/run_game.h src/run_game.cpp
        # The way in
        main.cpp
)

# The build picks where saves are kept, so no code asks at runtime.
if (EMSCRIPTEN)
    target_sources(cpp_warships PRIVATE src/build_save_archive_in_browser.cpp)
else()
    target_sources(cpp_warships PRIVATE src/build_save_archive_on_disk.cpp)
endif()

# Expose settings
target_include_directories(cpp_warships
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link dependencies
# warships_head_tui already carries the layers below it; naming them again only warns.
target_link_libraries(cpp_warships
        PRIVATE warships_head_tui
        PRIVATE cxx_setup
)

# Web build: an ES module exporting a factory, so a page wires up its own game.
if (EMSCRIPTEN)
    target_link_options(cpp_warships
            PRIVATE -pthread
            PRIVATE -sPROXY_TO_PTHREAD=1
            PRIVATE -sALLOW_MEMORY_GROWTH=1
            PRIVATE -sEXIT_RUNTIME=1
            PRIVATE -sMODULARIZE=1
            PRIVATE -sEXPORT_ES6=1
            PRIVATE -sEXPORT_NAME=createCppWarships
            PRIVATE -sENVIRONMENT=web,worker
            PRIVATE -sEXPORTED_RUNTIME_METHODS=FS,UTF8ToString,stringToNewUTF8
            PRIVATE -sEXPORTED_FUNCTIONS=_main,_ftxui_on_resize
    )
endif()
