cmake_minimum_required(VERSION 3.21...3.31)

project(glad
    VERSION 2.0.8
    LANGUAGES C
    HOMEPAGE_URL "https://github.com/Dav1dde/glad/"
    DESCRIPTION "Multi-Language Vulkan/GL/GLES/EGL/GLX/WGL Loader-Generator based on the official specs.")

add_library(glad STATIC src/gl.c include/glad/gl.h include/KHR/khrplatform.h)
target_include_directories(glad PUBLIC include)