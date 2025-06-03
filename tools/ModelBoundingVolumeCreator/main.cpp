#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include "Labyrinth/Helpers/compilerErrors.hpp"
#include "Labyrinth/engine.hpp"
#include "Labyrinth/window.hpp"
#include "Labyrinth/Engine/Graphics/bullet3Debug.hpp"
#include "Labyrinth/Engine/Graphics/regularShader.hpp"
#include "Labyrinth/player.hpp"

LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH();
#include <imgui.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <bullet/btBulletDynamicsCommon.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

LP_PRAGMA_DISABLE_ALL_WARNINGS_POP();

#include "BulletShape.hpp"

#include "Labyrinth/Engine/Resource/resourceManager.hpp"

#include "MeshContainer.hpp"

#include <imgui_internal.h> //for BeginDrapDropTargetWindow()

namespace
{
    void opengl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, [[maybe_unused]]  GLsizei length, GLchar const* message, [[maybe_unused]] void const* user_param);

    btCollisionWorld::ClosestRayResultCallback RayTestObtain(btDynamicsWorld* v_world, const lp::Player& cr_Player, lp::Window& r_window, const glm::vec2 cv_mousePos);

    /// @brief from https://github.com/ocornut/imgui/issues/5539
    bool BeginDrapDropTargetWindow(const char* payload_type)
    {
        using namespace ImGui;
        ImRect inner_rect = GetCurrentWindow()->InnerRect;
        if (BeginDragDropTargetCustom(inner_rect, GetID("##WindowBgArea")))
            if (const ImGuiPayload* payload = AcceptDragDropPayload(payload_type, ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
            {
                if (payload->IsPreview())
                {
                    ImDrawList* draw_list = GetForegroundDrawList();
                    draw_list->AddRectFilled(inner_rect.Min, inner_rect.Max, GetColorU32(ImGuiCol_DragDropTarget, 0.05f));
                    draw_list->AddRect(inner_rect.Min, inner_rect.Max, GetColorU32(ImGuiCol_DragDropTarget), 0.0f, 0, 2.0f);
                }
                if (payload->IsDelivery())
                    return true;
                EndDragDropTarget();
            }
        return false;
    }
}

#include "RigidBodyConteiner.hpp"

#include <bullet/Extras/Serialize/BulletWorldImporter/btBulletWorldImporter.h>

struct RendererForwardPlus_PlayerData
{
    glm::mat4 mView = {};
    glm::mat4 mProjection = {}; //infinite projection
    glm::vec3 mPosition = {};
    float padding = 0.0f;
};


int main()
{
    lp::Window window;
    if(window.create("Model BV Creator", 640, 480))
    {
        std::cerr << "ERROR: Window couldn't be created\n";
        return -1;
    }

    if(lp::g_engine.initialize())
    {
        std::cerr << "ERROR: Rngine initialization failed!\n";
        return -2;
    }

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, false);
    glDebugMessageCallback(opengl_message_callback, nullptr);
   
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    lp::Player mPlayer;
    

    lp::gl::Bullet3Debug bulletDebugRenderer;

     // Build the broadphase
     std::unique_ptr<btBroadphaseInterface> broadphase = std::make_unique<btDbvtBroadphase>();
    
     // Set up the collision configuration and dispatcher
     std::unique_ptr<btDefaultCollisionConfiguration> collisionConfiguration = std::make_unique<btDefaultCollisionConfiguration>();
     std::unique_ptr<btCollisionDispatcher> dispatcher = std::make_unique<btCollisionDispatcher>(collisionConfiguration.get());
 
     // The actual physics solver
     std::unique_ptr<btSequentialImpulseConstraintSolver> solver = std::make_unique<btSequentialImpulseConstraintSolver>();
  
     // The world.
     std::shared_ptr<btDiscreteDynamicsWorld> dynamicsWorld = std::make_shared<btDiscreteDynamicsWorld>(dispatcher.get(), broadphase.get(), solver.get(), collisionConfiguration.get());
     dynamicsWorld->setDebugDrawer(&bulletDebugRenderer);
     
     dynamicsWorld->setGravity(btVector3(0, 0, 0));
 
    std::unique_ptr<btRigidBody> vPhysicsBody;
    btCollisionShape* vPhysicsShape = new btSphereShape(1.0); //raw ptr for simplicity

    {
        std::unique_ptr<btDefaultMotionState> groundMotionState = std::make_unique<btDefaultMotionState>();
 
        btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0.001, groundMotionState.get(), vPhysicsShape, btVector3(0,0,0));
     
        vPhysicsBody =  std::make_unique<btRigidBody>(groundRigidBodyCI);
    }
    
    vPhysicsBody->setCollisionShape(vPhysicsShape); //use this???
    btMotionState* vvvsefsfd = new  btDefaultMotionState();
    vPhysicsBody->setMotionState(vvvsefsfd);
    vPhysicsBody->setCollisionFlags(vPhysicsBody->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
    vPhysicsBody->setCcdSweptSphereRadius(100.0);
    vPhysicsBody->setCcdMotionThreshold(1e-7);
    //https://docs.panda3d.org/1.10/python/programming/physics/bullet/ccd
    dynamicsWorld->addRigidBody(vPhysicsBody.get());


    GLuint VAO_model = 0;
        {
            glCreateVertexArrays(1, &VAO_model);
            glEnableVertexArrayAttrib(VAO_model, 0);
            glEnableVertexArrayAttrib(VAO_model, 1);
            glVertexArrayAttribFormat(VAO_model, 0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribFormat(VAO_model, 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
            glVertexArrayAttribBinding(VAO_model, 0, 0);
            glVertexArrayAttribBinding(VAO_model, 1, 0);
        }

    lpt::RigidBosyContainerCreationClassThing RBCCCT((std::shared_ptr<btDynamicsWorld>)dynamicsWorld);

    RendererForwardPlus_PlayerData playerData;

    GLuint UBOplayerDataBuffer = 0;
    glCreateBuffers(1, &UBOplayerDataBuffer);
    glNamedBufferStorage(UBOplayerDataBuffer, sizeof(RendererForwardPlus_PlayerData), nullptr, GL_DYNAMIC_STORAGE_BIT); 

    lp::res::ModelID_t graphics_model = lp::res::const_id_model_invalid;

    GLuint mVertexArrayModelTextured = 0;

            glCreateVertexArrays(1, &mVertexArrayModelTextured);
            glEnableVertexArrayAttrib(mVertexArrayModelTextured, 0);
            glEnableVertexArrayAttrib(mVertexArrayModelTextured, 1);
            glEnableVertexArrayAttrib(mVertexArrayModelTextured, 2);
            glEnableVertexArrayAttrib(mVertexArrayModelTextured, 3);
            glVertexArrayAttribFormat(mVertexArrayModelTextured, 0, 3, GL_FLOAT, GL_FALSE, 0);
            glVertexArrayAttribFormat(mVertexArrayModelTextured, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
            glVertexArrayAttribFormat(mVertexArrayModelTextured, 2, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float));
            glVertexArrayAttribFormat(mVertexArrayModelTextured, 3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float));
            glVertexArrayAttribBinding(mVertexArrayModelTextured, 0, 0);
            glVertexArrayAttribBinding(mVertexArrayModelTextured, 1, 0);
            glVertexArrayAttribBinding(mVertexArrayModelTextured, 2, 0);
            glVertexArrayAttribBinding(mVertexArrayModelTextured, 3, 0);

    lpt::MeshContainer meshContl;

    bool PLAYER_CollisionEnabled = true;
    bool IMGUI_ShowDemoWindow = false;
    bool IMGUI_ShowRigidBosyContainerWindow = false;
    bool IMGUI_ShowMeshContainerWindow = true;
    bool TRIGGER_drawDebug = true;

    double lastFrameTime = glfwGetTime();
    while(!window.shouldClose())
    {
        window.pollEvents();

        const double deltaTime = glfwGetTime() - lastFrameTime;
        lastFrameTime = glfwGetTime();
       // mPlayer.update(deltaTime);

        lp::g_engine.getResurceManager().getModelLoaderRef().update(1.0/60.0);

        {
            vPhysicsBody->clearForces();
            const glm::vec3 playerPos = mPlayer.getPosition();
            vPhysicsBody->getWorldTransform().setOrigin(btVector3(playerPos.x, playerPos.y, playerPos.z));
            vPhysicsBody->setLinearVelocity(btVector3(0, 0, 0));
            vPhysicsBody->setTurnVelocity(btVector3(0, 0, 0));
            vPhysicsBody->activate();
            
        }
        {
            playerData.mPosition = mPlayer.getPosition();
            playerData.mView = mPlayer.getViewMatrix();
            int width = 0, height = 0;
            window.debugGetFramebufferSize(width, height);
            glViewport(0, 0, width, height);
            playerData.mProjection = mPlayer.getProjectionMatrix((double)width / (double)height);

            glNamedBufferSubData(UBOplayerDataBuffer, 0, sizeof(RendererForwardPlus_PlayerData), &playerData);
        }

        ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_PassthruCentralNode);

        {
            ImGui::BeginMainMenuBar();
            if(ImGui::BeginMenu("File"))
            {
                if(ImGui::Button("Save"))
                {
                    btDefaultSerializer* serializer = new btDefaultSerializer();
                    serializer->serializeName("TEST_NAME");

                    if(PLAYER_CollisionEnabled) dynamicsWorld->removeCollisionObject(vPhysicsBody.get());

                    const auto& coa = dynamicsWorld->getCollisionObjectArray();
                    for(int i = 0; i < coa.size(); ++i)
                    {
                        std::cout << "Saving a " << coa.at(i)->getCollisionShape()->getName() << "\n";
                        //coa.at(i)->serializeSingleObject(serializer);
                    }
                    //coa.at(0)->setUserIndex(0);
                    dynamicsWorld->serialize(serializer);
                    
                    serializer->serializeName("TEST_NAME -- 2");
                    FILE* file = fopen("bullet_res.bullet", "wb");
                    fwrite(serializer->getBufferPointer(),serializer->getCurrentBufferSize(),1, file);
                    fclose(file);
                    delete serializer;

                    if(PLAYER_CollisionEnabled) dynamicsWorld->addRigidBody(vPhysicsBody.get());
                }
                ImGui::SetItemTooltip("Save the current project");
                if(ImGui::Button("Load"))
                {
                    RBCCCT.killAllChildren();
                    
                    btBulletWorldImporter* fileLoader = new btBulletWorldImporter(dynamicsWorld.get());
                    // fileLoader->setVerboseMode(15); //prints info about file to stdout
                    fileLoader->loadFile("bullet_res.bullet");
                    std::cout << "Loaded " <<  fileLoader->getNumCollisionShapes() << " Collision Shapes\n";
                    std::cout << "Loaded " <<  fileLoader->getNumRigidBodies() << " Rigid Bodies\n";
                    for(int i = 0; i < fileLoader->getNumRigidBodies(); ++i)
                    {
                        std::cout << "\tThe " << i << "th rigid body is " << fileLoader->getRigidBodyByIndex(i)->getCollisionShape()->getName() <<" \n";
                        btRigidBody* ptrrr =  dynamic_cast<btRigidBody*>(fileLoader->getRigidBodyByIndex(i));
                        
                        if(ptrrr == nullptr)
                        {
                            std::cout << " dynamic_cast<btRigidBody*> == nullptr!\n";
                        } else{
                            RBCCCT.addNewChild(ptrrr);
                        }
                        
                    }
                    
                    //It loads
                    // perfectly! (scale, position & all!!)
                    // so now we just somehow figure out WHAT we just loaded??
                    // maybe add a "New" button to delete all previous Shapes
                    // TODO: User data like the indices/pointer etc... DOES NOT GET SERIALIZED:
                    // Option 1: somehow put that functionality INTO bullet
                    // Option 2: don't bother and use the fact that they are serialized in the order they were added
                    //  - and we CAN serialize singlular objects
                    //  - we do not have ANY id'ing options in bullet
                    //  - but: Do we need one?
                    //  - theoretically, all of this is just to create ONE Compound Shape (upon load? something about children of a compound being duplicated also as separate shapes?)
                    //  - so no, we do not need to be able to know what object is what
                    //  - we will, however, have a problem with converting the bullet file into a format understood by this tool
                    //  - So the project Save/Load will save entire bullet file + json data (id/order -> name+stuff)
                    //  - And the Export/Import will should use the same data format as the game itself
                    //  - make a window to change params of resulting file -> game
                    //  - and do the rendering!!!
                    // https://github.com/bulletphysics/bullet3/blob/master/examples/Importers/ImportBullet/SerializeSetup.cpp
                    // https://pybullet.org/Bullet/BulletFull/classbtBulletWorldImporter.html#details
                    // https://web.archive.org/web/20170708145601/http://bulletphysics.org/mediawiki-1.5.8/index.php/Bullet_binary_serialization#Known_Limitations_or_Bugs
                    // https://web.archive.org/web/20170607040920/http://bulletphysics.org/mediawiki-1.5.8/index.php/Canonical_Game_Loop
                    // https://web.archive.org/web/20170707011812/http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Collision_Filtering <- free cam by setting filter to unused value??
                    // https://web.archive.org/web/20170602122143/http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Collision_Callbacks_and_Triggers <- important!!!

                    //https://web.archive.org/web/20170713085058/http://bulletphysics.org/mediawiki-1.5.8/index.php/Installation <- cmake params for main bullet CMakeLists <- read & think!!!
                    //https://web.archive.org/web/20170705111641/http://www.bulletphysics.org/mediawiki-1.5.8/index.php/Creating_a_project_from_scratch <- compilation of bullet
                }
                
                ImGui::SetItemTooltip("Load the project from a file");
                ImGui::BeginDisabled();
                if(ImGui::Button("Import"))
                {
                }
                ImGui::SetItemTooltip("Import a model from a file");
                if(ImGui::Button("Export"))
                {
                }
                ImGui::SetItemTooltip("Export current model");
                ImGui::EndDisabled();
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Windows"))
            {
                ImGui::Checkbox("Demo Window", &IMGUI_ShowDemoWindow);
                ImGui::Checkbox("RigidBody Container ? Window", &IMGUI_ShowRigidBosyContainerWindow);
                ImGui::Checkbox("Mesh Container Window", &IMGUI_ShowMeshContainerWindow);
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Model"))
            {
                static char model_text_path[256] = {};
                ImGui::InputTextWithHint("model path", "File path to the model", model_text_path, 255);
                if(ImGui::Button("Load model"))
                {
                    graphics_model = lp::g_engine.getResurceManager().loadModel(model_text_path);
                    //load the model in here
                    // put the model loader into a new file
                    // make a new shader so we don't load the textures
                    // like have it be one color, but change lighting on normals + camera dir?
                    // the point is storing + displaying the model as close to as-is as possible
                    // at some point in the future make a proper loader ui
                    // with ALL the settings (assimp + stuff)
                    // have it be only on this thread
                    // but maybe put in separate thread in the future
                    // that said just use the provided model load function for now
                    // THEN create bullet file saving + loading
                    //  - figure out the order / how to tell which body is which
                    //  - so we can load them in the right order
                    //  - bullet DOES store user data ...
                    //  - but does that get serialized?
                    //  - Last resort is make serialization myself
                }
                ImGui::SetItemTooltip("Load a new model, unloading the previous one");
                ImGui::EndMenu();
            }
            if(ImGui::BeginMenu("Camera"))
            {
                float speed = mPlayer.getSpeedRef();
                if(ImGui::SliderFloat("Speed", &speed, 0.001, 100.0))
                {
                    mPlayer.getSpeedRef() = speed;
                }
                ImGui::SetItemTooltip("The players' camera speed");

                int flag = vPhysicsBody->getCollisionFlags();
                if(ImGui::CheckboxFlags("Stop visualizing the Player Collider", &flag, btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT))
                {
                    vPhysicsBody->setCollisionFlags(flag);
                }
                ImGui::SetItemTooltip("Stops the players' camera btCollisionShape from being visible in the Debug Drawer");

                if(ImGui::Checkbox("Enable collision", &PLAYER_CollisionEnabled))
                {
                    if(PLAYER_CollisionEnabled)
                    {
                        dynamicsWorld->addRigidBody(vPhysicsBody.get());
                    } else
                    {
                        dynamicsWorld->removeRigidBody(vPhysicsBody.get());
                    }
                }
                ImGui::SetItemTooltip("Enable collision for the players' camera");
                ImGui::Checkbox("Draw Debug", &TRIGGER_drawDebug);
                ImGui::SetItemTooltip("Draw Bullet debug");
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
        

        if(ImGui::Begin("Basics"))
        {
            {
                const btVector3 scale = vPhysicsShape->getLocalScaling();
                float scale_v[3] = {scale.getX(), scale.getY(), scale.getZ()};
                if(ImGui::SliderFloat3("Scale", scale_v, 0.01f, 10.0f))
                {
                    vPhysicsShape->setLocalScaling(btVector3(scale_v[0], scale_v[1], scale_v[2]));
                }

                ImGui::Text("Mouse: x = %f, y = %f", ImGui::GetIO().MousePos.x, ImGui::GetIO().MousePos.y);
                {
                    int height = 0, width = 0;
                    window.getWindowSize(width, height);
                    ImGui::Text("Window: x = %d, y = %d", width, height);
                }
               
                //ImGui::GetIO().MouseClicked[0]

            }
            //vPhysicsBody->getCenterOfMassPosition(); //TODO: make a move() func for a given btRigidBody (teleport, maybe interpolation as separate?)
            ImGui::Text("Name: \"%s\"", vPhysicsShape->getName());
        }
        ImGui::End();

        if(IMGUI_ShowDemoWindow) ImGui::ShowDemoWindow(&IMGUI_ShowDemoWindow);

        if(IMGUI_ShowRigidBosyContainerWindow)
        {
            if(ImGui::Begin("RigidBody Container ?", &IMGUI_ShowRigidBosyContainerWindow))
            {
                
                if(BeginDrapDropTargetWindow("MESH_TYPE"))
                {
                    const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MESH_TYPE");
                    if(payload != nullptr && payload->DataSize == sizeof(lpt::MeshDragDropID))
                    {
                        lpt::MeshDragDropID* ptr = (lpt::MeshDragDropID*)payload->Data;
                        std::cout << "Dropped a " << ptr->mModel << " @ " << ptr->mID << "\n";
                        
                            const auto* mmesh = meshContl.getMesh(*ptr);
                            if(mmesh != nullptr)
                            {
                                btIndexedMesh meshBT;
                                meshBT.m_numTriangles = mmesh->getIndiciesVector().size() / 3;
                                meshBT.m_triangleIndexBase = (const unsigned char*)mmesh->getIndiciesVector().data();
                                meshBT.m_triangleIndexStride = 3 * sizeof(unsigned int);
                                meshBT.m_numVertices = mmesh->getPositionsVector().size();
                                meshBT.m_vertexBase = (const unsigned char*)mmesh->getPositionsVector().data();
                                meshBT.m_vertexStride = sizeof(glm::vec3);
                                meshBT.m_vertexType = PHY_FLOAT;
                                btTriangleIndexVertexArray *interf = new btTriangleIndexVertexArray(); //can't kill this, memory leak FIXME TODO
                                interf->addIndexedMesh(meshBT);
                                btBvhTriangleMeshShape* shape = new btBvhTriangleMeshShape(interf, true);
                                btMotionState* mstate = new btDefaultMotionState();
                                btRigidBody* body = new btRigidBody(0.0, mstate, shape);
                                RBCCCT.addNewChild(body);
                                dynamicsWorld->addRigidBody(body); //https://www.google.com/search?client=firefox-b-d&q=bullet+physics+one+sided+triangle+mesh+collision    <- note

                                std::cout << "Object added successfully\n";
                            } else std::cerr << "Dropped an invalid mesh!\n";
                            
                        
                        
                    }
                    ImGui::EndDragDropTarget();
                }
                //This works!!
                // need to differentiate between a STATIC btCollisionShape & a dynamic/kinematic one
                // only a static body can have a btTriangleMesh; btBvhTriangleMeshShape; ?
                // but apparently there MAY be others
                // TODO: btConvexHullShape <- what meshes can be this?
                // - upon getting payload get its reference (make function)
                // - show a ui??
                //   + popup modal?
                //   + regular window?
                // - this will then create this

                /// TODO: investigate the aiProcess_PreTransformVertices flag <- may fix out transorm issue
                ///  Adding on to that, remove all unused data from being imported by assimp
                // https://github.com/ocornut/imgui/wiki/Multi-Select
                

                RBCCCT.drawUI();
            }
            ImGui::End();
        }
        if(IMGUI_ShowMeshContainerWindow) meshContl.ui(&IMGUI_ShowMeshContainerWindow);

        //ImGui mesh modification window goes here
        //it modifies a mesh/its copy
        // - shows info?
        // - drag & drop onto it to use the new mesh
        // - modal popup if wrong

        // @see https://www.opengl-tutorial.org/miscellaneous/clicking-on-objects/picking-with-a-physics-library/
        //if imgui doesn't want the mouse AND left mouse button was pressed
        if(!ImGui::GetIO().WantCaptureMouse && ImGui::GetIO().MouseClicked[0]) 
        {
            auto RayCallback = RayTestObtain(dynamicsWorld.get(), mPlayer, window, glm::vec2((float)ImGui::GetIO().MousePos.x, (float)ImGui::GetIO().MousePos.y));
            if(RayCallback.hasHit())
            {
                std::cout << "BANG!\n";
            } else
            {
                std::cout << "Wooosh!\n";
            }   
        }




        // make function/window TO:
            // - create rigidbody (shape) - all the usable ones
            // - have a window for vPhysicsShape - specific settings (for all usable shapes)
            // - have a window for all vPhysicsBody - important settings (IMPORTANT ONES!!!)

        // make a menu to load a model & display it
        // make a menu to import/export files

        if(TRIGGER_drawDebug) dynamicsWorld->debugDrawWorld();
        dynamicsWorld->stepSimulation(deltaTime);

        {
            const btVector3 vec = vPhysicsBody->getWorldTransform().getOrigin();
            mPlayer.setPosition({vec.getX(), vec.getY(), vec.getZ()});
        }

        glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE); //set clipping plane to [0,1], instead of the default [-1,1] 
        glDepthFunc(GL_GREATER); //these 3 reverse the depth buffer
        glClearDepth(0); //clear depth to 0
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //sets the clear colour to black (red)
        
        //https://eliasdaler.wordpress.com/2016/01/07/using-lua-with-c-in-practice-part4/
        //https://github.com/NoahStolk/simple-level-editor
        //https://noelberry.ca/posts/making_games_in_2025/
        //https://developer.valvesoftware.com/wiki/Valve_Hammer_Editor#Extensions
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK); //cull back faces
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindBufferBase(GL_UNIFORM_BUFFER, 0, UBOplayerDataBuffer);

        
        if(TRIGGER_drawDebug == true && bulletDebugRenderer.getBuffer() != 0)
        {
            lp::gl::RegularShader shader;
            shader.LoadShader(lp::gl::ShaderType::DebugLine);
            shader.Use();
            shader.SetUniform(3, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));

            glBindVertexArray(VAO_model);
            glVertexArrayVertexBuffer(VAO_model, 0, bulletDebugRenderer.getBuffer(), 0, 6 * sizeof(float));
            glDrawArrays(GL_LINES, 0, bulletDebugRenderer.getDrawCount());
            glBindVertexArray(0);
        }

        if(graphics_model != lp::res::const_id_model_invalid)
        {
            auto* modelLoaded = lp::g_engine.getResurceManager().getLoadedModel(graphics_model);
            if(modelLoaded != nullptr)
            {
                lp::gl::RegularShader shader;
                shader.LoadShader(lp::gl::ShaderType::ModelTextured);
                shader.Use();
                shader.SetUniform(3, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f)));

                lp::res::LoadedModel::MaterialID_t lastMaterial = 4'000'000'000u; //hopefully we will never a model with this many materials

                glBindVertexArray(mVertexArrayModelTextured);
                for(const auto &i : modelLoaded->mMeshes)
                {
                    if(lastMaterial != i.mMaterialID)
                    {
                        modelLoaded->mMaterials[i.mMaterialID].mColor.Bind(0);
                    }
                    //std::cout << "VAO: " << mVertexArrayModelTextured << ", VBO: " << i.mVBO << ", EBO: " << i.mEBO << ", DrawCount: " << i.mDrawCount << "\n";
                    glVertexArrayVertexBuffer(mVertexArrayModelTextured, 0, i.mVBO, 0, sizeof(lp::res::VertexFull));
                    glVertexArrayElementBuffer(mVertexArrayModelTextured, i.mEBO);
                    glDrawElements(GL_TRIANGLES, i.mDrawCount, GL_UNSIGNED_INT, nullptr);
                }
                glBindVertexArray(0);
                lp::gl::Texture::Unbind(0); //unbind color texture
            }
        }
        window.swapBuffers();
    }

    lp::g_engine.destroy();
    window.destroy();
}
    /**
     * TODO: eventually add fullscreen functionality in Window
     * 
     * TODO: functionality of THIS:
     *  - ONE model at a time
     *  - can load any model from file with assimp
     *  - simplest possible graphics
     *  - UI with ImGui
     *  - ONE BoundingVolume class type at a time 
     *  - choose with dropdown?
     *  - than functions sepcific to this wil appear
     *  - ways to TIE physics <-> graphics
     *  - SAVE to file
     *  - LOAD from file (removing all prior data)
     *  - try to avoid globals so we can maybe have this in game proper?
     * 
     * TODO: functions of ALL TOOLS:
     *  - save/load models/physics/sounds to/from file
     *  - every model like this will be a .json file with refs to .bullet + .glfw + (sound like .ogg)
     *  - a way to create:
     *  - STATIC geometry for levels
     *  - DYNAMIC (scripted wit lua) stuff for levels (like gates/doors, buttons, levers)
     *  
     * 
     *  - Figure out some kind of interpolation system for anims (NOT skeletal, 'root' movement only)
     * 
     * 
     * TODO: 
     *  - create new (.gitignore'd) bin/ directory
     *  https://stackoverflow.com/questions/73763985/set-cmake-output-executable-directory
     *  https://stackoverflow.com/questions/543203/cmake-runtime-output-directory-on-windows (comment by hgyxbll)
     *  https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#build-configurations
     *  https://cfd.university/learn/automating-cfd-solver-and-library-compilation-using-cmake/advanced-cmake-features-to-power-up-your-cfd-development/   <- cmake pseudo tutorial (ctest, cpack, etc...)
     * 
     * https://cfd.university/learn/keep-your-users-happy-how-to-document-code-the-right-way/documenting-code-why-bother-and-how-to-do-it-right/
     * 
     *  Set LIBRARY_OUTPUT_DIRECTORY directory in preset?
     *  I want to be able to run my program by running:
     *  ./bin/Release/ OR ./bin/Debug/ ??? or just one for both?
     *  have them be separate, better
     *  Make this work with Visual Studio!!!
     * CMakePresets.json release-base is broken +
     * CMAKE_INSTALL_PREFIX <- outdated, + what is this???
     */

namespace
{
    btCollisionWorld::ClosestRayResultCallback RayTestObtain(btDynamicsWorld* v_world, const lp::Player& cr_Player, lp::Window& r_window, const glm::vec2 cv_mousePos)
    {
        int height = 0, width = 0;
        r_window.getWindowSize(width, height);

        // The ray Start and End positions, in Normalized Device Coordinates (Have you read Tutorial 4 ?)
        glm::vec4 lRayStart_NDC(
            (cv_mousePos.x/(float)width  - 0.5f) * 2.0f, // [0,1024] -> [-1,1]
            (cv_mousePos.y/(float)height - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
            -1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
            1.0f
        );
        glm::vec4 lRayEnd_NDC(
            (cv_mousePos.x/(float)width  - 0.5f) * 2.0f,
            (cv_mousePos.y/(float)height - 0.5f) * 2.0f,
            0.001, //was 0.0, but caused lRayEnd_world.w == 0, which caused division by 0, this fixes it
            1.0f
        );
        glm::mat4 InverseProjectionMatrix = {};
        {
            int FRAM_width = 0, FRAM_height = 0;
            r_window.debugGetFramebufferSize(FRAM_width, FRAM_height);
            InverseProjectionMatrix = cr_Player.getProjectionMatrix((double)FRAM_width / (double)FRAM_height);

            // The Projection matrix goes from Camera Space to NDC.
            // So inverse(ProjectionMatrix) goes from NDC to Camera Space.
            // InverseProjectionMatrix = glm::inverse(InverseProjectionMatrix);
        }
        // The View Matrix goes from World Space to Camera Space.
        // So inverse(ViewMatrix) goes from Camera Space to World Space.
        //glm::mat4 InverseViewMatrix = glm::inverse(mPlayer.getViewMatrix());

        // glm::vec4 lRayStart_camera = InverseProjectionMatrix * lRayStart_NDC;    lRayStart_camera/=lRayStart_camera.w;
        // glm::vec4 lRayStart_world  = InverseViewMatrix       * lRayStart_camera; lRayStart_world /=lRayStart_world .w;
        // glm::vec4 lRayEnd_camera   = InverseProjectionMatrix * lRayEnd_NDC;      lRayEnd_camera  /=lRayEnd_camera  .w;
        // glm::vec4 lRayEnd_world    = InverseViewMatrix       * lRayEnd_camera;   lRayEnd_world   /=lRayEnd_world   .w;

        // Faster way (just one inverse)
        glm::mat4 M = glm::inverse(InverseProjectionMatrix * cr_Player.getViewMatrix());
        glm::vec4 lRayStart_world = M * lRayStart_NDC; lRayStart_world/=lRayStart_world.w;
        
        glm::vec4 lRayEnd_world   = M * lRayEnd_NDC;
        //std::cout << "lRayEnd_world - before division - x = " << lRayEnd_world.x << " y = " << lRayEnd_world.y << " z = " << lRayEnd_world.z << " w = " << lRayEnd_world.w << "\n"; 
        //if(lRayEnd_world.w != 0.0f)
        lRayEnd_world  /=lRayEnd_world.w;

        //std::cout << "lRayEnd_world x = " << lRayEnd_world.x << " y = " << lRayEnd_world.y << " z = " << lRayEnd_world.z << " w = " << lRayEnd_world.w << "\n"; 
        //std::cout << "lRayEnd_NDC x = " << lRayEnd_NDC.x << " y = " << lRayEnd_NDC.y << " z = " << lRayEnd_NDC.z << " w = " << lRayEnd_NDC.w << "\n"; 

        glm::vec3 lRayDir_world(lRayEnd_world - lRayStart_world);
        lRayDir_world = glm::normalize(lRayDir_world);

        glm::vec3 out_origin = lRayStart_world; 
        glm::vec3 out_direction = glm::normalize(lRayDir_world);

        //glm::vec3 out_origin = cr_Player.getPosition(), out_direction = glm::normalize(cr_Player.getViewDirection()); //works, but only for 'middle' of screen, mouse is not accounted for

        glm::vec3 out_end = out_origin + out_direction*1000.0f;

        //std::cout << "From x = " << out_origin.x << " y = " << out_origin.y << " z = " << out_origin.z << "\n";
        //std::cout << "To x = " << out_end.x << " y = " << out_end.y << " z = " << out_end.z << "\n"; 

        btCollisionWorld::ClosestRayResultCallback RayCallback(
            btVector3(out_origin.x, out_origin.y, out_origin.z), 
            btVector3(out_end.x, out_end.y, out_end.z)
        );

        v_world->rayTest(
            btVector3(out_origin.x, out_origin.y, out_origin.z), 
            btVector3(out_end.x, out_end.y, out_end.z), 
            RayCallback
        );
        return RayCallback;
    }


    void opengl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, [[maybe_unused]]  GLsizei length, GLchar const* message, [[maybe_unused]] void const* user_param)
    {
        auto const src_str = [source]() {
            switch (source)
            {
            case GL_DEBUG_SOURCE_API: return "API";
            case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
            case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
            case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
            case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
            case GL_DEBUG_SOURCE_OTHER: return "OTHER";
            default: return "BAD SOURCE";
            }
            }();

        auto const type_str = [type]() {
            switch (type)
            {
            case GL_DEBUG_TYPE_ERROR: return "ERROR";
            case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
            case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
            case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
            case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
            case GL_DEBUG_TYPE_MARKER: return "MARKER";
            case GL_DEBUG_TYPE_OTHER: return "OTHER";
            default: return "BAD ERROR TYPE";
            }
            }();

        auto const severity_str = [severity]() {
            switch (severity)
            {
            case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
            case GL_DEBUG_SEVERITY_LOW: return "LOW";
            case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
            case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
            default: return "BAD SEVERITY";
            }
            }();

        std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << "\n";

    }
}