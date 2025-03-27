cmake_minimum_required(VERSION 3.21...3.31)

project(CDF_LABIRYNTH_PENDING
    VERSION 0.0.1
    LANGUAGES CXX
    DESCRIPTION "A game made from scratch about a foggy labyrinth.")

set(CDF_SOURCES #list of all source files in the src/ directory
    main.cpp
    )

set(CDF_HEADERS #list of all header files in the includes/Labyrinth/ directory
    test_header.hpp 
    )

list(TRANSFORM CDF_SOURCES PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/src/)
list(TRANSFORM CDF_HEADERS PREPEND ${CMAKE_CURRENT_SOURCE_DIR}/includes/Labyrinth/)

add_executable(Labyrinth ${CDF_SOURCES} ${CDF_HEADERS})
target_include_directories(Labyrinth PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/includes)
target_compile_features(Labyrinth PRIVATE cxx_std_20)
