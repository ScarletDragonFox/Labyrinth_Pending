cmake_minimum_required(VERSION 3.21...3.31)

add_library(dependancies INTERFACE)


# add GLFW library
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_LIBRARY_TYPE STATIC CACHE INTERNAL "" FORCE)

add_subdirectory(glfw)

# add assimp library
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build shared libraries" FORCE)
set(ASSIMP_BUILD_SAMPLES OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
 
add_subdirectory(assimp)

#add glm library
add_subdirectory(glm)

#add stb library
add_subdirectory(stb)

#add glad library
add_subdirectory(glad)

#add imgui library
add_subdirectory(imgui)

#add nlohmann json library
set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(json)

target_link_libraries(dependancies INTERFACE glfw assimp glm stb glad imgui nlohmann_json::nlohmann_json)
