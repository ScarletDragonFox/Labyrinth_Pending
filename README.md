# Labyrinth_Pending
A game made from scratch about a foggy labyrinth.



## notes:

### **main.cpp**:


figure out what audio formats we can work with: <br>
do a window with glfw & glad & imgui to test that out <br>
fix bullet3:
 - get the src/ directory from git (manually? or get cmake to do it)
 - get list of all .c / .cpp files to compile
 - copy all headers to {build_dir}/include/ (with respect to folders) (find way to automate, maybe with GLOB)
 - leave notes of what this does and stuff in a deps/bullet3/README.md file <- for maintnance & stuff
 
Create documantation:
 - STYLE.ms <- how to write code so that all code is (relatively) consistant 
 - Read how to make & write a LICENSE.md
 - create new CMakeLists.txt in docs/ to run doxygen wit hright settings & create docs in docs/gen/
 - update main README.md describing every library we are using (see LICENSE.md)

### **ECS/coreECS.hpp**:

 - create a logging system and tie it to everything! (send requirements (neccessities; nice-to-haves; optional features 
    like reusability by e.g. using stdlib for writng to a location 
         (the streams so you can have a custom one , or use stdout, or stderr)))
 - maybe some kind of reset function???
 - and MAYYYYYBE saving all this to and from a file??????
 - get collegue to test system for holes!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 - get cmake working in visual studio, or maybe just clone the repo and with cmake-gui generate a Visual Studio project
 - with that check this for memory usage & if an intensive test is made, see performance bottlenecks!!!!