# Compile files
add_executable(cpp_warships main.cpp
        include/AttackHandler.h src/AttackHandler.cpp
        include/BattleController.h src/BattleController.cpp
        include/BattleView.h src/BattleView.cpp
        include/ComputerPlayer.h src/ComputerPlayer.cpp
        include/FieldCellView.h src/FieldCellView.cpp
        include/GameController.h
        include/GameField.h src/GameField.cpp
        include/GameFieldView.h src/GameFieldView.cpp
        include/GamePauseView.h src/GamePauseView.cpp
        include/MatchSettings.h src/MatchSettings.cpp
        include/MatchSettingsController.h src/MatchSettingsController.cpp
        include/PlaceShipController.h src/PlaceShipController.cpp
        include/PlaceShipControllerView.h src/PlaceShipControllerView.cpp
        include/Player.h src/Player.cpp
        include/PlayerFieldState.h src/PlayerFieldState.cpp
        include/Segment.h src/Segment.cpp
        include/Ship.h src/Ship.cpp
        include/ShipManager.h src/ShipManager.cpp
        include/ShipManagerView.h src/ShipManagerView.cpp
        include/Structures.h src/Structures.cpp
        include/exceptions/BattleException.h src/exceptions/BattleException.cpp
        include/exceptions/ShipPlacementException.h src/exceptions/ShipPlacementException.cpp
        # Player Skills
        include/player_skills/DoubleDamage.h src/player_skills/DoubleDamage.cpp
        include/player_skills/Scanner.h src/player_skills/Scanner.cpp
        include/player_skills/ShootingRandomlySkill.h src/player_skills/ShootingRandomlySkill.cpp
        include/player_skills/Skill.h src/player_skills/Skill.cpp
        include/player_skills/SkillFactory.h
        include/player_skills/SkillManager.h src/player_skills/SkillManager.cpp
        include/player_skills/SkillManagerView.h src/player_skills/SkillManagerView.cpp
        include/player_skills/exceptions/SkillException.h src/player_skills/exceptions/SkillException.cpp
        # Save System
        include/save_system/GameFieldJsonSerializer.h src/save_system/GameFieldJsonSerializer.cpp
        include/save_system/GameSaveCreator.h src/save_system/GameSaveCreator.cpp
        include/save_system/GameStateJsonSerializer.h src/save_system/GameStateJsonSerializer.cpp
        include/save_system/JsonFileHandler.h src/save_system/JsonFileHandler.cpp
        include/save_system/MatchSettingsJsonSerializer.h src/save_system/MatchSettingsJsonSerializer.cpp
        include/save_system/SegmentJsonSerializer.h src/save_system/SegmentJsonSerializer.cpp
        include/save_system/ShipJsonSerializer.h src/save_system/ShipJsonSerializer.cpp
        include/save_system/ShipManagerJsonSerializer.h src/save_system/ShipManagerJsonSerializer.cpp
        include/save_system/SkillManagerJsonSerializer.h src/save_system/SkillManagerJsonSerializer.cpp
        # State Machine
        include/state_machine/GameState.h src/state_machine/GameState.cpp
        include/GameStateDTO.h src/GameStateDTO.cpp
        include/state_machine/MatchBuilder.h src/state_machine/MatchBuilder.cpp
        include/state_machine/states/MenuGameState.h src/state_machine/states/MenuGameState.cpp
        include/state_machine/states/OngoingGameState.h src/state_machine/states/OngoingGameState.cpp
        include/state_machine/states/ShutdownGameState.h src/state_machine/states/ShutdownGameState.cpp
        include/state_machine/states/StateContext.h src/state_machine/states/StateContext.cpp
        include/state_machine/states/TransitGameState.h src/state_machine/states/TransitGameState.cpp
        include/state_machine/substates/BattleOngoingGameSubState.h src/state_machine/substates/BattleOngoingGameSubState.cpp
        include/state_machine/substates/FinishOngoingGameSubState.h src/state_machine/substates/FinishOngoingGameSubState.cpp
        include/state_machine/substates/InitiateOngoingGameSubState.h src/state_machine/substates/InitiateOngoingGameSubState.cpp
        include/state_machine/substates/NewMatchSettingsSubState.h src/state_machine/substates/NewMatchSettingsSubState.cpp
        include/state_machine/substates/OngoingGameSubState.h src/state_machine/substates/OngoingGameSubState.cpp
        include/state_machine/substates/PauseOngoingGameSubState.h src/state_machine/substates/PauseOngoingGameSubState.cpp
        include/state_machine/substates/SubStateContext.h src/state_machine/substates/SubStateContext.cpp
)

# Link dependencies
target_link_libraries(cpp_warships PRIVATE
        nlohmann_json::nlohmann_json
        ftxui::ftxui
        utilities
        input_parser
        input_reader
        game_saves
        cxx_setup
)
