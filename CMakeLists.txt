cmake_minimum_required(VERSION 3.21...3.31)

project(CDF_LABIRYNTH_PENDING
    VERSION 0.0.2
    LANGUAGES CXX
    DESCRIPTION "A game made from scratch about a foggy labyrinth.")

# 'dependancies' target is from here
# has to be added BEFORE src/
add_subdirectory(deps) 

# 'lp_source_library' target is from here
# it used the 'dependancies' target, so put this AFTER add_subdirectory(deps)
# The 'lp_source_library' target contains all info required to compileL
# flags, include directories, libraries & source files EXCEPT src/main.cpp (which contains main function)
# allows for inheretance, used in tools/ and tests/
add_subdirectory(src)


add_executable(Labyrinth src/main.cpp)
target_link_libraries(Labyrinth PRIVATE lp_source_library)