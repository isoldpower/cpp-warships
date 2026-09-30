include(FetchContent)

# Never re-fetch what is already on disk: a failed update wipes the source tree, and
# pinned tags never move anyway.
set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE BOOL "" FORCE)

# Include libraries on testing
if (BUILD_TESTING)
    FetchContent_Declare(googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG b514bdc898e2951020cbdca1304b75f5950d1f59
            GIT_SHALLOW TRUE)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif ()

# Include JSON library
FetchContent_Declare(nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.12.0
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(nlohmann_json)


# Include FTXUI for Terminal UI
FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
        GIT_TAG v7.0.0
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(ftxui)


# The browser half of the save storage, linked in as JavaScript.
if (EMSCRIPTEN)
    add_library(browser_save_storage_binding INTERFACE)
    target_link_options(browser_save_storage_binding INTERFACE
            "SHELL:--js-library ${CMAKE_CURRENT_SOURCE_DIR}/web/browser_save_storage.js")
endif()
