# TODO, or a file with ideas to possibly include

### **ECS/coreECS.hpp**:

 - create a logging system and tie it to everything! (send requirements (neccessities; nice-to-haves; optional features 
    like reusability by e.g. using stdlib for writng to a location 
         (the streams so you can have a custom one , or use stdout, or stderr)))
 - maybe some kind of reset function???
 - and MAYYYYYBE saving all this to and from a file??????
 - get collegue to test system for holes!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 - get cmake working in visual studio, or maybe just clone the repo and with cmake-gui generate a Visual Studio project
 - with that check this for memory usage & if an intensive test is made, see performance bottlenecks!!!!

 ### **game.cpp**:
do we want physics to run at a constant framerate?
 - yes, bullet does it for us.

 1. get the inputs <- keyboard, maybe even controller
 2. process said inputs (raw key in / mouse in -> move player in dir X, move camera in dir Y)
    a. abstracting input from action (camera movement) is neccessary for having more than 1 way of doing it
    b. IF we go with 2 seperate threads, 1 for main/physics/logic and the other for stricte rendering, this would allow us 
    c. to have all inputs purely in the main window thread
    d. so then we could have separate threads for:
       - purely rendering (the rendering thread)
       - purely window / glfw stuff (the main / window thread)
       - physics / other logic code (the processing thread)
    e. we would need to seperate the threads near completely
    f. we could do that with the EVENT SYSTEM:
       - main thread reads input, processes it and sends out a PLAYER.CAMERA.MOVE event
       - the processing thread reads that event, and moves the camera, updating the frustum & stuff
       - the processing thread finishes processing this frame, and sends it to rensering via a SCENE.READY event
       - the rendering thread gets the scene and renderes it
   g. Problems:
       - complex graph
       - requires a load of synchronisation schenanigans 
       - absurdly complex event manager system
       - how do you handle the scene?
         + data needs loading into OpenGL
         + who will 'own' the data?
         + how do you ahndle scene changes?
         + create a new scene every time, or have a buffer???  

    ./build/gccWindowsRelease/Labyrinth.exe to run exe    


### **main.cpp**:

figure out what audio formats we can work with: <br>
do a window with glfw & glad & imgui to test that out <br>
fix bullet3:
 - get the src/ directory from git (manually? or get cmake to do it)
 - get list of all .c / .cpp files to compile
 - copy all headers to {build_dir}/include/ (with respect to folders) (find way to automate, maybe with GLOB)
 - leave notes of what this does and stuff in a deps/bullet3/README.md file <- for maintnance & stuff
 
Create documantation:
 - STYLE.md <- how to write code so that all code is (relatively) consistant 
 - Read how to make & write a LICENSE.md
 - create new CMakeLists.txt in docs/ to run doxygen wit hright settings & create docs in docs/gen/
 - update main README.md describing every library we are using (see LICENSE.md)

To stuffz:
 - Support for Visual Studio
 - create headers for bullet -> glm too


 


```
#include <nlohmann/json.hpp>
#include <lua.h>
```
https://lucasklassmann.com/blog/2023-02-26-more-advanced-examples-of-embedding-lua-in-c/ <br>
https://lucasklassmann.com/blog/2019-02-02-embedding-lua-in-c/ <br>
https://www.katsbits.com/site/collision-models/ <br>
https://mrscientist.itch.io/3d-low-poly-modular-dungeon <br>
https://styloo.itch.io/dungeon-asset-pack <br>
https://itch.io/game-assets/free/tag-3d/tag-dungeon-crawler <br>
https://json-schema.org/learn/getting-started-step-by-step <br>
https://gist.github.com/mcleary/b0bf4fa88830ff7c882d <br>
https://github.com/methylDragon/coding-notes/blob/master/C++/07%20C++%20-%20Threading%20and%20Concurrency.md <br>
https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#S-cpl <br>