cmake_minimum_required(VERSION 3.21...3.31)

project(imgui
    VERSION 1.91.9
    LANGUAGES CXX
    HOMEPAGE_URL "https://github.com/ocornut/imgui/"
    DESCRIPTION "Dear ImGui: Bloat-free Graphical User interface for C++ with minimal dependencies")

set(IMGUI_SOURCE_FILES 
    "src/imgui_demo.cpp"
    "src/imgui_draw.cpp"
    "src/imgui_impl_glfw.cpp"
    "src/imgui_stdlib.cpp"
    "src/imgui_tables.cpp"
    "src/imgui_widgets.cpp"
    "src/imgui.cpp"
)

set(IMGUI_HEADER_FILES 
    "include/imgui_stdlib.h"
    "include/imstb_rectpack.h"
    "include/imstb_textedit.h"
    "include/imstb_truetype.h"
    "include/imconfig.h"
    "include/imgui.h"
    "include/imgui_impl_glfw.h"
    "include/imgui_impl_opengl3.h"
    "include/imgui_internal.h"
)

add_library(imgui INTERFACE)
target_sources( imgui INTERFACE ${IMGUI_SOURCE_FILES})
target_include_directories(imgui INTERFACE include)