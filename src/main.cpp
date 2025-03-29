#include <Labyrinth/test_header.hpp>

#include <GLFW/glfw3.h>
#include <assimp/Importer.hpp>
#include <glm/glm.hpp>
#include <stb_image.h>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui.h>

#include <nlohmann/json.hpp>
#include <bullet/btBulletCollisionCommon.h>

#include <lua.h>

int main()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    lp::print("Hello, world!");
    ImGui::DestroyContext();
}