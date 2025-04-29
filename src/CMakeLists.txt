cmake_minimum_required(VERSION 3.21...3.31)
# File that stores the list of source files + headers

set(CDF_SOURCES #list of all source files in the src/ directory
#    main.cpp # File added back in by the 'Labyrinth' main target, compiles codebase as the game
    Engine/eventManager.cpp

    Engine/ECS/coreECS.cpp
    Engine/ECS/entityManager.cpp
    Engine/ECS/systemManager.cpp

    Engine/Resource/resourceManager.cpp
    Engine/Resource/shaderManager.cpp

    Engine/Resource/modelLoader.cpp

    Engine/Graphics/textures.cpp
    Engine/Graphics/regularShader.cpp

    Engine/Graphics/framebuffer.cpp
    Engine/Graphics/basicFrowardRenderer.cpp

    game.cpp
    engine.cpp
    player.cpp
    window.cpp
    )

set(CDF_HEADERS #list of all header files in the includes/Labyrinth/ directory (optional, could be empty with no consequence)
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

    Engine/Resource/modelLoader.hpp
    Engine/Resource/loadedModel.hpp

    Engine/Graphics/types.hpp
    Engine/Graphics/textures.hpp
    Engine/Graphics/regularShader.hpp

    Engine/Graphics/framebuffer.hpp
    Engine/Graphics/basicFrowardRenderer.hpp


    game.hpp
    engine.hpp
    player.hpp
    window.hpp
    )

    # include(CMakePrintHelpers)

    # message("Before list")

    # cmake_print_variables(${CDF_SOURCES})
    # cmake_print_variables(${CDF_HEADERS})

# Add path to source files
# Source Files are listed without it for convinience
list(TRANSFORM CDF_SOURCES PREPEND ${PROJECT_SOURCE_DIR}/src/)
list(TRANSFORM CDF_HEADERS PREPEND ${PROJECT_SOURCE_DIR}/includes/Labyrinth/)

add_library(lp_source_library STATIC ${CDF_SOURCES} ${CDF_HEADERS})

target_compile_features(lp_source_library PUBLIC cxx_std_20)

target_include_directories(lp_source_library PUBLIC ${PROJECT_SOURCE_DIR}/includes)

# Quick Fix for CDF_COMPILE_FLAGS list being interpreted by target_compile_options as one string
string(REPLACE " " ";" CDF_COMPILE_FLAGS_FIXED ${CDF_COMPILE_FLAGS})
target_compile_options(lp_source_library PUBLIC ${CDF_COMPILE_FLAGS_FIXED})

# Reason why this file MUST by added AFTER deps/
# the 'dependancies' target wouldn't be defined, throwing en error
target_link_libraries(lp_source_library PUBLIC dependancies)