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
//https://lucasklassmann.com/blog/2023-02-26-more-advanced-examples-of-embedding-lua-in-c/
//https://lucasklassmann.com/blog/2019-02-02-embedding-lua-in-c/

#include <soloud.h>
#include <soloud_speech.h>
#include <soloud_thread.h>

int main()
{
    //IMGUI_CHECKVERSION();
    //ImGui::CreateContext();
    lp::print("Hello, world!");
    //ImGui::DestroyContext();


    // Define a couple of variables
    SoLoud::Soloud soloud;  // SoLoud engine core
    SoLoud::Speech speech;  // A sound source (speech, in this case)

    // Configure sound source
    speech.setText("1 2 3   1 2 3   Hello world. Welcome to So-Loud.");

    // initialize SoLoud.
    soloud.init();

    // Play the sound source (we could do this several times if we wanted)
    soloud.play(speech);

    // Wait until sounds have finished
    while (soloud.getActiveVoiceCount() > 0)
    {
        // Still going, sleep for a bit
        SoLoud::Thread::sleep(100);
    }

    // Clean up SoLoud
    soloud.deinit();

    // All done.
    return 0;
}