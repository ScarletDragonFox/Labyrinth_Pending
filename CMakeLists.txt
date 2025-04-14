cmake_minimum_required(VERSION 3.21...3.31)

project(CDF_LABIRYNTH_PENDING
    VERSION 0.0.1
    LANGUAGES CXX
    DESCRIPTION "A game made from scratch about a foggy labyrinth.")

set(CDF_SOURCES #list of all source files in the src/ directory
    main.cpp
    Engine/eventManager.cpp

    Engine/ECS/coreECS.cpp
    Engine/ECS/entityManager.cpp
    Engine/ECS/systemManager.cpp

    Engine/Resource/resourceManager.cpp
    Engine/Resource/shaderManager.cpp

    Engine/Graphics/textures.cpp
    Engine/Graphics/regularShader.cpp

    Engine/Graphics/framebuffer.cpp
    Engine/Graphics/basicFrowardRenderer.cpp


    game.cpp
    engine.cpp
    player.cpp
    )

set(CDF_HEADERS #list of all header files in the includes/Labyrinth/ directory
    Engine/event.hpp
    Engine/eventManager.hpp
    Engine/types.hpp

    Engine/ECS/baseSystem.hpp
    Engine/ECS/componentContainer.hpp
    Engine/ECS/componentManager.hpp
    Engine/ECS/coreECS.hpp
    Engine/ECS/entityManager.hpp
    Engine/ECS/systemManager.hpp
    Engine/ECS/types.hpp

    Engine/Resource/resourceManager.hpp
    Engine/Resource/shaderManager.hpp
    
    Engine/Graphics/types.hpp
    Engine/Graphics/textures.hpp
    Engine/Graphics/regularShader.hpp

    Engine/Graphics/framebuffer.hpp
    Engine/Graphics/basicFrowardRenderer.hpp


    game.hpp
    engine.hpp
    player.hpp
    )

list(TRANSFORM CDF_SOURCES PREPEND src/)
list(TRANSFORM CDF_HEADERS PREPEND includes/Labyrinth/)



add_executable(Labyrinth ${CDF_SOURCES} ${CDF_HEADERS})

target_compile_features(Labyrinth PRIVATE cxx_std_20)
target_include_directories(Labyrinth PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/includes)

target_compile_definitions(Labyrinth PRIVATE GLM_FORCE_DEPTH_ZERO_TO_ONE)

string(REPLACE " " ";" CDF_COMPILE_FLAGS_FIXED ${CDF_COMPILE_FLAGS})
target_compile_options(Labyrinth PRIVATE ${CDF_COMPILE_FLAGS_FIXED})

add_subdirectory(deps)

target_link_libraries(Labyrinth PRIVATE dependancies)