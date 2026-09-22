# Compile files
add_library(game_tui
        # Presentation vocabulary
        include/game_tui/Theme.h src/game_tui/Theme.cpp
        include/game_tui/CellAppearance.h src/game_tui/CellAppearance.cpp
        include/game_tui/ScreenKind.h
        include/game_tui/MatchQuery.h
        # What a screen may ask for
        include/game_tui/Intent.h
        include/game_tui/intents/ShowScreenIntent.h
        include/game_tui/intents/StartMatchIntent.h
        include/game_tui/intents/ChangeThemeIntent.h
        include/game_tui/intents/PlaceShipIntent.h
        include/game_tui/intents/RemoveShipIntent.h
        include/game_tui/intents/ShuffleFleetIntent.h
        include/game_tui/intents/BeginBattleIntent.h
        include/game_tui/intents/QuitIntent.h
        src/game_tui/Intents.cpp
        # Turning input into actions
        include/game_tui/EventHandler.h
        include/game_tui/EventRouter.h src/game_tui/EventRouter.cpp
        # Reusable pieces of a view
        include/game_tui/views/KeyHint.h src/game_tui/views/KeyHint.cpp
        include/game_tui/views/BoardView.h src/game_tui/views/BoardView.cpp
        # Screens
        include/game_tui/Screen.h
        include/game_tui/screens/MenuState.h
        include/game_tui/screens/MenuEventHandlers.h src/game_tui/screens/MenuEventHandlers.cpp
        include/game_tui/screens/MenuScreen.h src/game_tui/screens/MenuScreen.cpp
        include/game_tui/screens/PlacementState.h src/game_tui/screens/PlacementState.cpp
        include/game_tui/screens/PlacementView.h src/game_tui/screens/PlacementView.cpp
        include/game_tui/screens/PlacementEventHandlers.h
        src/game_tui/screens/PlacementEventHandlers.cpp
        include/game_tui/screens/PlacementScreen.h src/game_tui/screens/PlacementScreen.cpp
        # The running interface
        include/game_tui/Application.h src/game_tui/Application.cpp
)

# Tests setup
if (BUILD_TESTING)
    add_executable(game_tui_tests tests/main.cpp)
    target_link_libraries(game_tui_tests PRIVATE GTest::gtest_main game_tui)
    include(GoogleTest)
    gtest_discover_tests(game_tui_tests)
endif()

# Expose settings
target_include_directories(game_tui
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Link dependencies
target_link_libraries(game_tui
        PUBLIC game_core game_flow game_persistence ftxui::ftxui cxx_setup
)
