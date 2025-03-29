cmake_minimum_required(VERSION 3.21...3.31)

project(stb
    VERSION 2.3.0
    LANGUAGES CXX
    HOMEPAGE_URL "https://github.com/nothings/stb/"
    DESCRIPTION "stb single-file public domain libraries for C/C++")

add_library(stb STATIC source.cpp include/stb_image_write.h include/stb_image.h)
target_include_directories(stb PUBLIC include)