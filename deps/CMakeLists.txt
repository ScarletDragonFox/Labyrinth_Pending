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
set(ASSIMP_WARNINGS_AS_ERRORS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "we will never export any models" FORCE)
# ASSIMP_OPT_BUILD_PACKAGES <- for CPack???
# ASSIMP_DOUBLE_PRECISION <- double precision??

 
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

#add bullet3 library
add_subdirectory(bullet3)

#add lua library
add_subdirectory(lua) 

#add soloud library
add_subdirectory(soloud) 

target_link_libraries(dependancies INTERFACE glfw assimp glm stb glad imgui nlohmann_json::nlohmann_json bullet3 lua::lib soloud)

#add global #defines
target_compile_definitions(dependancies INTERFACE GLM_FORCE_DEPTH_ZERO_TO_ONE)