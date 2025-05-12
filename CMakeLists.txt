cmake_minimum_required(VERSION 3.21...3.31)

project(CDF_LABIRYNTH_PENDING
    VERSION 0.0.2
    LANGUAGES CXX
    DESCRIPTION "A game made from scratch about a foggy labyrinth.")

# Makes some generators like Visual Studio use folders
# Is ON by default in cmake 3.26
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# 'dependancies' target is from here
# has to be added BEFORE src/
add_subdirectory(deps) 

# 'lp_source_library' target is from here
# The 'lp_source_library' target contains all info required to compile the main exec', outside of the main func
# flags, include directories, libraries & source files EXCEPT src/main.cpp (which contains main function)
# allows for inheretance, used in tools/ and tests/
# REMEMBER TO target_link_libraries BOTH lp_source_library AND dependancies for EVERY target
# Done this way to avoid duplicate libraries in linker
add_subdirectory(src)


add_executable(Labyrinth src/main.cpp)
target_link_libraries(Labyrinth PRIVATE lp_source_library dependancies)

#BUILD_TOOLS - builds other executables/utilities
set(BUILD_TOOLS ON CACHE BOOL "Build additional tools" FORCE)

if(BUILD_TOOLS)
    add_subdirectory(tools)
endif()