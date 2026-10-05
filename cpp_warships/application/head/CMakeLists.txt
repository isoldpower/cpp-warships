# The interface, in two halves.
#
# warships_head is everything that decides what the game looks like and what the player
# meant by a keystroke, together with the plain text presentation. It must build and link
# without any drawing library at all. The plain renderers live in this target rather than
# beside the FTXUI ones precisely so that the build proves it: leak an FTXUI include into
# common/ and this target stops compiling, with no separate check to remember to run.
add_library(warships_head
        # Shared vocabulary
        include/application/head/common/Color.h
        include/application/head/common/Theme.h src/application/head/common/Theme.cpp
        include/application/head/common/Queries.h
        include/application/head/common/ScreenKind.h
        include/application/head/common/ThemeSelection.h src/application/head/common/ThemeSelection.cpp
        include/application/head/common/PresentationContext.h src/application/head/common/PresentationContext.cpp
        # What the interface remembers of its own
        include/application/head/common/state/MenuState.h
        include/application/head/common/state/PlacementState.h src/application/head/common/state/PlacementState.cpp
        include/application/head/common/state/BattleState.h src/application/head/common/state/BattleState.cpp
        include/application/head/common/state/SaveBrowserState.h
        include/application/head/common/state/SaveNamingState.h
        include/application/head/common/state/PresentationState.h
        include/application/head/common/state/PanelState.h
        # Reading input: raw strokes in, asks in the game's own words out
        include/application/head/common/input/Keystroke.h src/application/head/common/input/Keystroke.cpp
        include/application/head/common/input/KeyCodes.h src/application/head/common/input/KeyCodes.cpp
        include/application/head/common/input/GridGeometry.h src/application/head/common/input/GridGeometry.cpp
        include/application/head/common/input/ScreenRegion.h
        include/application/head/common/input/PanelScrolling.h src/application/head/common/input/PanelScrolling.cpp
        include/application/head/common/input/ScreenInput.h
        include/application/head/common/input/InputKey.h
        include/application/head/common/input/BoundScreenInput.h
        src/application/head/common/input/BoundScreenInput.cpp
        include/application/head/common/input/keys/MoveCursorKey.h
        src/application/head/common/input/keys/MoveCursorKey.cpp
        include/application/head/common/input/keys/PanelKeys.h
        src/application/head/common/input/keys/PanelKeys.cpp
        include/application/head/common/input/keys/LeaveToMenuKey.h
        src/application/head/common/input/keys/LeaveToMenuKey.cpp
        include/application/head/common/input/keys/MenuKeys.h
        src/application/head/common/input/keys/MenuKeys.cpp
        include/application/head/common/input/keys/PlacementKeys.h
        src/application/head/common/input/keys/PlacementKeys.cpp
        include/application/head/common/input/keys/BattleKeys.h
        src/application/head/common/input/keys/BattleKeys.cpp
        include/application/head/common/input/keys/SaveBrowserKeys.h
        src/application/head/common/input/keys/SaveBrowserKeys.cpp
        include/application/head/common/input/keys/SaveNamingKeys.h
        src/application/head/common/input/keys/SaveNamingKeys.cpp
        include/application/head/common/input/MenuInput.h src/application/head/common/input/MenuInput.cpp
        include/application/head/common/input/PlacementInput.h src/application/head/common/input/PlacementInput.cpp
        include/application/head/common/input/BattleInput.h src/application/head/common/input/BattleInput.cpp
        include/application/head/common/input/SaveBrowserInput.h
        src/application/head/common/input/SaveBrowserInput.cpp
        include/application/head/common/input/SaveNamingInput.h
        src/application/head/common/input/SaveNamingInput.cpp
        include/application/head/common/input/EventBus.h src/application/head/common/input/EventBus.cpp
        include/application/head/common/input/EventPipeline.h src/application/head/common/input/EventPipeline.cpp
        # ... and what the game does about them
        include/application/head/common/input/handlers/SessionHandlers.h
        src/application/head/common/input/handlers/SessionHandlers.cpp
        include/application/head/common/input/handlers/PlacementHandlers.h
        src/application/head/common/input/handlers/PlacementHandlers.cpp
        include/application/head/common/input/handlers/BattleHandlers.h
        src/application/head/common/input/handlers/BattleHandlers.cpp
        # How anything is drawn at all
        include/application/head/common/render/Frame.h src/application/head/common/render/Frame.cpp
        include/application/head/common/render/Renderer.h
        include/application/head/common/render/RendererSet.h src/application/head/common/render/RendererSet.cpp
        include/application/head/common/render/Notices.h src/application/head/common/render/Notices.cpp
        include/application/head/common/render/CoordinateLabel.h src/application/head/common/render/CoordinateLabel.cpp
        include/application/head/common/render/EventNarration.h src/application/head/common/render/EventNarration.cpp
        # Where it runs
        include/application/head/common/host/Shell.h
        # ... and how it all looks with no drawing library to hand
        include/application/head/plain/PlainFrame.h src/application/head/plain/PlainFrame.cpp
        include/application/head/plain/PlainBoardGrid.h src/application/head/plain/PlainBoardGrid.cpp
        include/application/head/plain/PlainMenuView.h src/application/head/plain/PlainMenuView.cpp
        include/application/head/plain/PlainPlacementView.h src/application/head/plain/PlainPlacementView.cpp
        include/application/head/plain/PlainBattleView.h src/application/head/plain/PlainBattleView.cpp
        include/application/head/plain/PlainSaveBrowserView.h
        src/application/head/plain/PlainSaveBrowserView.cpp
        include/application/head/plain/PlainSaveNamingView.h
        src/application/head/plain/PlainSaveNamingView.cpp
        include/application/head/plain/TerminalShell.h src/application/head/plain/TerminalShell.cpp
)

# warships_head_tui is the only place in the project that names FTXUI.
add_library(warships_head_tui
        include/application/head/tui/FtxuiPalette.h src/application/head/tui/FtxuiPalette.cpp
        include/application/head/tui/FtxuiView.h src/application/head/tui/FtxuiView.cpp
        include/application/head/tui/FtxuiNotices.h src/application/head/tui/FtxuiNotices.cpp
        include/application/head/tui/CellAppearance.h src/application/head/tui/CellAppearance.cpp
        include/application/head/tui/KeyHint.h src/application/head/tui/KeyHint.cpp
        include/application/head/tui/BoardView.h src/application/head/tui/BoardView.cpp
        include/application/head/tui/ScrollPanel.h src/application/head/tui/ScrollPanel.cpp
        include/application/head/tui/ScreenParts.h src/application/head/tui/ScreenParts.cpp
        include/application/head/tui/DialogView.h src/application/head/tui/DialogView.cpp
        include/application/head/tui/MenuView.h src/application/head/tui/MenuView.cpp
        include/application/head/tui/PlacementView.h src/application/head/tui/PlacementView.cpp
        include/application/head/tui/BattleView.h src/application/head/tui/BattleView.cpp
        include/application/head/tui/SaveBrowserView.h src/application/head/tui/SaveBrowserView.cpp
        include/application/head/tui/SaveNamingView.h src/application/head/tui/SaveNamingView.cpp
        include/application/head/tui/TuiShell.h src/application/head/tui/TuiShell.cpp
)

# Tests setup
if (BUILD_TESTING)
    add_executable(warships_head_tests
            tests/GridGeometryTests.cpp
            tests/KeystrokeTests.cpp
            tests/KeyCodeTests.cpp
            tests/KeyboardLayoutTests.cpp
            tests/PanelScrollingTests.cpp
            tests/PresentationContextTests.cpp
            tests/RenderTests.cpp
            tests/ScreenInputTests.cpp
            tests/ThemeTests.cpp)
    target_link_libraries(warships_head_tests PRIVATE GTest::gtest_main warships_head)
    include(GoogleTest)
    gtest_discover_tests(warships_head_tests)

    add_executable(warships_head_tui_tests
            tests/TuiLayoutTests.cpp)
    target_link_libraries(warships_head_tui_tests PRIVATE GTest::gtest_main warships_head_tui)
    gtest_discover_tests(warships_head_tui_tests)
endif()

# Expose settings
target_include_directories(warships_head PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(warships_head_tui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Link dependencies. Note what is missing from warships_head: no drawing library appears
# here or anywhere below it.
target_link_libraries(warships_head
        PUBLIC warships_core warships_flow warships_persistence warships_model cxx_setup
)
target_link_libraries(warships_head_tui
        PUBLIC warships_head ftxui::ftxui cxx_setup platform
)
