cmake_minimum_required(VERSION 3.21...3.31)

project(Tool_ModelBoundingVolumeCreator
    VERSION 0.0.1
    LANGUAGES CXX
    DESCRIPTION "A tool to attach collsion to models & export it."
)

set(TOOL_SOURCE 
    main.cpp
    
    BulletShape.hpp
    BulletShape.cpp
)

add_executable(Tool_ModelBoundingVolumeCreator ${TOOL_SOURCE} )

set_target_properties(Tool_ModelBoundingVolumeCreator PROPERTIES LINK_INTERFACE_MULTIPLICITY 3)

target_link_libraries(Tool_ModelBoundingVolumeCreator PRIVATE lp_source_library dependancies)
set_target_properties(Tool_ModelBoundingVolumeCreator PROPERTIES OUTPUT_NAME "MBVCreator")
