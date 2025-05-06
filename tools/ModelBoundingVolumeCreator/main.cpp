#include <iostream>
#include <memory>

#include "Labyrinth/Helpers/compilerErrors.hpp"

#include "Labyrinth/engine.hpp"
#include "Labyrinth/window.hpp"

LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH();
#include <imgui.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <bullet/btBulletDynamicsCommon.h> //TODO: causes multiple definitions (.lib) with bullet???? FIXME!!!
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>


#include <bullet/BulletCollision/CollisionShapes/btCompoundShape.h>

LP_PRAGMA_DISABLE_ALL_WARNINGS_POP();

#include "Labyrinth/Engine/Graphics/bullet3Debug.hpp"
#include "Labyrinth/Engine/Graphics/regularShader.hpp"
#include "Labyrinth/player.hpp"

#include <array>
#include <vector>
#include <string>
#include <string_view>
#include <variant>

#include "BulletShape.hpp"

namespace
{
    void opengl_message_callback(GLenum source, GLenum type, GLuint id, GLenum severity, [[maybe_unused]]  GLsizei length, GLchar const* message, [[maybe_unused]] void const* user_param);

    btCollisionWorld::ClosestRayResultCallback RayTestObtain(btDynamicsWorld* v_world, const lp::Player& cr_Player, lp::Window& r_window, const glm::vec2 cv_mousePos);
}

template <typename T>
class TreeNode
{
    public:
    TreeNode()
    {
        data = new T;
    }
    // inline std::size_t addChild(T& child_data)
    // {
    //     TreeNode<T> node = TreeNode<T>(child_data);
    //     node.setName(name + "-" + std::to_string(mChildren.size()));
    //     mChildren.push_back(TreeNode<T>(child_data));
    //     return mChildren.size() - 1;
    // }
    inline std::size_t addChild()
    {
        TreeNode<T> node;
        node.setName(name + "-" + std::to_string(mChildren.size()));
        mChildren.push_back(node);
        return mChildren.size() - 1;
    }
    // if return 8008500 - not found
    inline std::size_t findChild(const std::string_view searchedName) const
    {
        for(std::size_t i = 0; i < mChildren.size(); ++i)
        {
            if(mChildren[i].getName() == searchedName)
            {
                return i;
            }
        }
        return 8008500;
    }

    void suicide()
    {
        for(auto& child:mChildren)
        {
            child.suicide();
        }
        if((bool)mDeathCallback) //retrurns true if mDeathCallback was assigned a value
        {
            std::cout << "Suocide!\n";
            mDeathCallback(*this);
        }
    }

    void killChild(const std::size_t child_id)
    {
        if(mChildren.size() <= child_id)
        {
            std::cerr << "TreeNode::killChild() ERROR: child_id too big!\n";
            return;
        }
        mChildren[child_id].suicide();
        mChildren.erase(mChildren.begin() + child_id);
    }

    void setDeathCallback(std::function<void(TreeNode<T>&)> v_callback)
    {
        mDeathCallback = v_callback;
    }

    std::vector<TreeNode<T>>& getChildren(){return mChildren;}
    void orphan(){mChildren.clear();}
    inline T& getData(){return *data;}
    std::string_view getName() const {return name;}
    void setName(std::string_view newName){name = newName;}

    ~TreeNode()
    {
        if(data) delete data;
    }

    private:
    std::function<void(TreeNode<T>&)> mDeathCallback;
    T* data;
    std::string name = "Node";
    std::vector<TreeNode<T>> mChildren;
};



void ImGui_DrawTreeNode(TreeNode<int>& node, TreeNode<int>* parent = nullptr)
{
    if(ImGui::TreeNode(node.getName().data()))
    {
        ImGui::SliderInt("value", &node.getData(), 0, 100);
        if(ImGui::Button("Create child"))
        {
            node.addChild();
        }
        ImGui::SameLine();
        if(ImGui::Button("Kill child"))
        {
            if(parent != nullptr)
            {
                parent->killChild(parent->findChild(node.getName()));
                //return early to avoid dead children.
                ImGui::TreePop();
                return;
            }
        }
        for(auto& child: node.getChildren())
        {
            ImGui_DrawTreeNode(child, &node);
        }
        ImGui::TreePop();
    }
}

struct TreeNode_Data
{
    //std::unique_ptr<btCollisionShape> mShape;
    btCollisionShape* mShape;
    // using GenericPtr = std::unique_ptr<btCollisionShape>;
    // using ContainerPtr = std::unique_ptr<btCompoundShape>;

    // std::variant<GenericPtr, ContainerPtr> mShape;
    
    lpt::BulletShapeType mType = lpt::BulletShapeType::Size;
    bool not_loaded = true;

    // TreeNode_Data()
    // {

    // }

    // TreeNode_Data(TreeNode_Data& cpy) = delete;
};

static TreeNode_Data* g_Modal_CreateNew_Shape_TreeNode_Data = nullptr;
// /// @brief called when Modal_CreateNew_Shape() has "OK" button pressed by user
// static std::function<void(TreeNode_Data*)> g_Modal_CreateNew_Shape_creation_callback_func;


// call by  ImGui::OpenPopup("Create New Shape - Modal");
void Modal_CreateNew_Shape()
{
    constexpr std::size_t val_previously_selected_default = static_cast<std::size_t>(lpt::BulletShapeType::SphereShape);
    static TreeNode_Data* out_data = nullptr;
    static std::size_t val_previously_selected = val_previously_selected_default;
    // 0 - radius ; boxHalfExtents.X ; radius
    // 1 -        ; boxHalfExtents.Y ; height
    // 2 -        ; boxHalfExtents.Z ;
    static float var_float[3] = {};

    
    if(out_data != g_Modal_CreateNew_Shape_TreeNode_Data)
    {
        //reset all values in here
        val_previously_selected = val_previously_selected_default;

        out_data = g_Modal_CreateNew_Shape_TreeNode_Data;
        var_float[0] = 1.0f;
        var_float[1] = 1.0f;
        var_float[2] = 1.0f;

        std::cout << "out_data = " <<(void*)out_data << "\n";
        std::cout << "g_Modal_CreateNew_Shape_TreeNode_Data = " <<(void*)g_Modal_CreateNew_Shape_TreeNode_Data << "\n";
    }
   
    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

    if(ImGui::BeginPopupModal("Create New Shape - Modal", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
    {
        // /*

        const char* val_preview = lpt::getName(static_cast<lpt::BulletShapeType>(val_previously_selected));
        if(ImGui::BeginCombo("btCollisionShape", val_preview))
        {
            for(std::size_t i = 0; i < static_cast<std::size_t>(lpt::BulletShapeType::Size); ++i)
            {
                const bool is_selected = (val_previously_selected == i);
                if(ImGui::Selectable(lpt::getName(static_cast<lpt::BulletShapeType>(i)), is_selected))
                {
                    val_previously_selected = i;
                }
            }
            ImGui::EndCombo();
        }

        const lpt::BulletShapeType enumType = static_cast<lpt::BulletShapeType>(val_previously_selected);

        switch(enumType)
        {
            case lpt::BulletShapeType::CompoundShape:
            {
                ImGui::Text("CompoundShape settings here");
                break;
            }

            case lpt::BulletShapeType::CapsuleShapeY:
            [[fallthrough]];
            case lpt::BulletShapeType::CapsuleShapeX:
            [[fallthrough]];
            case lpt::BulletShapeType::CapsuleShapeZ:
            [[fallthrough]];
            case lpt::BulletShapeType::ConeShapeY:
            [[fallthrough]];
            case lpt::BulletShapeType::ConeShapeX:
            [[fallthrough]];
            case lpt::BulletShapeType::ConeShapeZ:
            {
                ImGui::SliderFloat("radius", &var_float[0], 0.001f, 100.0f);
                ImGui::SliderFloat("height", &var_float[1], 0.001f, 100.0f);
                break;
            }

            case lpt::BulletShapeType::CylinderShapeY:
            [[fallthrough]];
            case lpt::BulletShapeType::CylinderShapeX:
            [[fallthrough]];
            case lpt::BulletShapeType::CylinderShapeZ:
            [[fallthrough]];
            case lpt::BulletShapeType::BoxShape:
            {
                ImGui::SliderFloat3("boxHalfExtents", &var_float[0], 0.001f, 100.0f);
                break;
            }
            case lpt::BulletShapeType::SphereShape:
            {
                ImGui::SliderFloat("radius", &var_float[0], 0.001f, 100.0f);
                break;
            }
            case lpt::BulletShapeType::ConvexHullShape:
            {
                ImGui::Text("ConvexHullShape settings here");
                ImGui::Text("ConvexHullShape model load here?");
                ImGui::Text("maybe even model optimisation??");
                break;
            }

            case lpt::BulletShapeType::Size:
            [[fallthrough]];
            default:
                ImGui::Text("Impossible value chosen. Quo vadis?");
                break;
        }

        if (ImGui::Button("OK", ImVec2(120, 0))) 
        {
            std::cout << "HERE - 0!\n";
            std::cout << "out_data = " <<(void*)out_data << "\n";
            std::cout << "g_Modal_CreateNew_Shape_TreeNode_Data = " <<(void*)g_Modal_CreateNew_Shape_TreeNode_Data << "\n";
            //out_data; //fill this out in here
            out_data->mType = enumType;
            out_data->not_loaded = false;
            std::cout << "HERE - 1!\n";

            std::cout << "Creating a " << lpt::getName(enumType) << "\n";

            switch(enumType)
            {
                case lpt::BulletShapeType::CompoundShape:
                {
                    break;
                }
                case lpt::BulletShapeType::CapsuleShapeY:
                {
                    //out_data->mShape = std::make_unique<btCapsuleShape>(var_float[0], var_float[1]);
                    out_data->mShape = new btCapsuleShape(var_float[0], var_float[1]);
                    break;
                }
                case lpt::BulletShapeType::CapsuleShapeX:
                {
                    //out_data->mShape = std::make_unique<btCapsuleShapeX>(var_float[0], var_float[1]);
                    out_data->mShape = new btCapsuleShapeX(var_float[0], var_float[1]);
                    break;
                }
                case lpt::BulletShapeType::CapsuleShapeZ:
                {
                    //out_data->mShape = std::make_unique<btCapsuleShapeZ>(var_float[0], var_float[1]);
                    out_data->mShape = new btCapsuleShapeZ(var_float[0], var_float[1]);
                    break;
                }

                case lpt::BulletShapeType::ConeShapeY:
                {
                    //out_data->mShape = std::make_unique<btConeShape>(var_float[0], var_float[1]);
                    out_data->mShape = new btConeShape(var_float[0], var_float[1]);
                    break;
                }
                case lpt::BulletShapeType::ConeShapeX:
                {
                    //out_data->mShape = std::make_unique<btConeShapeX>(var_float[0], var_float[1]);
                    out_data->mShape = new btConeShapeX(var_float[0], var_float[1]);
                    break;
                }
                case lpt::BulletShapeType::ConeShapeZ:
                {
                    //out_data->mShape = std::make_unique<btConeShapeZ>(var_float[0], var_float[1]);
                    out_data->mShape = new btConeShapeZ(var_float[0], var_float[1]);
                    break;
                }

                case lpt::BulletShapeType::CylinderShapeY:
                {
                    //out_data->mShape = std::make_unique<btCylinderShape>(btVector3(var_float[0], var_float[1], var_float[2]));
                    out_data->mShape = new btCylinderShape(btVector3(var_float[0], var_float[1], var_float[2]));
                    break;
                }
                case lpt::BulletShapeType::CylinderShapeX:
                {
                    //out_data->mShape = std::make_unique<btCylinderShapeX>(btVector3(var_float[0], var_float[1], var_float[2]));
                    out_data->mShape = new btCylinderShapeX(btVector3(var_float[0], var_float[1], var_float[2]));
                    break;
                }
                case lpt::BulletShapeType::CylinderShapeZ:
                {
                    //out_data->mShape = std::make_unique<btCylinderShapeZ>(btVector3(var_float[0], var_float[1], var_float[2]));
                    out_data->mShape = new btCylinderShapeZ(btVector3(var_float[0], var_float[1], var_float[2]));
                    break;
                }

                case lpt::BulletShapeType::BoxShape:
                {
                    //out_data->mShape = std::make_unique<btBoxShape>(btVector3(var_float[0], var_float[1], var_float[2]));
                    out_data->mShape = new btBoxShape(btVector3(var_float[0], var_float[1], var_float[2]));
                    break;
                }

                case lpt::BulletShapeType::SphereShape:
                {
                    //std::unique_ptr<btCollisionShape> ptr = std::make_unique<btSphereShape>(var_float[0]);
                    out_data->mShape = new btSphereShape(var_float[0]);
                    std::cout << "Created, saving!\n";
                    out_data->mType = lpt::BulletShapeType::SphereShape;
                    std::cout << "Created  actually\n";

                    //std::unique_ptr<btSphereShape>  <- what this was orignally, didn't change anything!!!!
                    //out_data->mShape = std::move(ptr); //SIGSEGV <- segfault-s
                    //ptr.release();
                    //EVERY SINGLE ONE CRASES!!
                    // maybe move this into a class, in a file of its own???
                    // the refactoring may solve this problem
                    // but every single one crashing means the problem must be either:
                    // - moving the pointer, somehow 
                    //    (the bullet class seem to be a bit hit-or-miss with the virtual destructor, 
                    //        althought at a glance all classes with data members of their own have them?)
                    // - out_data, despite earlier write being successfull.
                    // Either way, moving this into a class/file will require enough changes that it might just work???
                    // NOTE TO SELF: rendering in main app can be done parallel to this, so if this is too hard ...
                    std::cout << "Created, saved!\n";
                    break;
                }

                case lpt::BulletShapeType::ConvexHullShape:
                {
                    break;
                }

                case lpt::BulletShapeType::Size:
                [[fallthrough]];
                default:
                    std::cerr << "Queo Vadis?\n";
                    break;
                }
            std::cout << "HERE - 1-a!\n";
            ImGui::CloseCurrentPopup();
            g_Modal_CreateNew_Shape_TreeNode_Data = nullptr;
            std::cout << "HERE - 1-b!\n";
        }
        ImGui::SameLine();
        //*/
        if (ImGui::Button("Cancel", ImVec2(120, 0))) 
        { 
            ImGui::CloseCurrentPopup();
            g_Modal_CreateNew_Shape_TreeNode_Data = nullptr;
        }
        ImGui::EndPopup();
    }
    
};

void ImGui_DrawTreeNode(TreeNode<TreeNode_Data>& node, TreeNode<TreeNode_Data>* parent = nullptr)
{
    const bool child_not_yet_alive = node.getData().not_loaded;
    if(child_not_yet_alive) ImGui::BeginDisabled();
    if(ImGui::TreeNode(node.getName().data()))
    {
        auto& node_data = node.getData();
        ImGui::Text("Name: \"%s\"", node_data.mShape->getName());

        if(ImGui::Button("Create child"))
        {
            const auto death_callback = [parent](TreeNode<TreeNode_Data>& dcb_v_node) -> void
            {
                auto& c_data = dcb_v_node.getData();
                auto& p_data = parent->getData();
                if(p_data.mType != lpt::BulletShapeType::CompoundShape)
                {
                    std::cerr << "parent of a node is not a legal parent. How?\n";
                    return;
                }
                btCompoundShape* pptr = dynamic_cast<btCompoundShape*>(p_data.mShape);
                if(pptr != nullptr)
                {
                    pptr->removeChildShape(c_data.mShape);
                } else
                {
                    std::cout << "dynamic_cast is broken!\n";
                }
                if(dcb_v_node.getData().mShape)
                {
                    delete dcb_v_node.getData().mShape;
                    dcb_v_node.getData().mShape = nullptr;
                }
                
                //pptr->getChildList();
            };
            std::size_t id = node.addChild();
            auto& child_child = node.getChildren().at(id);
            child_child.setDeathCallback(death_callback);
            g_Modal_CreateNew_Shape_TreeNode_Data = &child_child.getData();

            ImGui::OpenPopup("Create New Shape - Modal");
            
            std::cout << "Popup should've been created!\n";
        }
        ImGui::SameLine();
        if(ImGui::Button("Kill child"))
        {
            if(parent != nullptr)
            {
                parent->killChild(parent->findChild(node.getName()));
                //return early to avoid dead children.
                ImGui::TreePop();
                return;
            }
        }
        for(auto& child: node.getChildren())
        {
            ImGui_DrawTreeNode(child, &node);
        }
        ImGui::TreePop();
    }
    if(child_not_yet_alive) ImGui::EndDisabled();

    Modal_CreateNew_Shape(); //needs to be here for ID to match
}

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
     std::unique_ptr<btDiscreteDynamicsWorld> dynamicsWorld = std::make_unique<btDiscreteDynamicsWorld>(dispatcher.get(), broadphase.get(), solver.get(), collisionConfiguration.get());
     dynamicsWorld->setDebugDrawer(&bulletDebugRenderer);
     
     dynamicsWorld->setGravity(btVector3(0, 0, 0));
 
    std::unique_ptr<btRigidBody> vPhysicsBody;
    btCollisionShape* vPhysicsShape = new btSphereShape(1.0); //raw ptr for simplicity

    {
        std::unique_ptr<btDefaultMotionState> groundMotionState = std::make_unique<btDefaultMotionState>();
 
        btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState.get(), vPhysicsShape, btVector3(0,0,0));
     
        vPhysicsBody =  std::make_unique<btRigidBody>(groundRigidBodyCI);
    }
    
    vPhysicsBody->setCollisionShape(vPhysicsShape); //use this???

    vPhysicsBody->setMassProps(0.0, btVector3(0.0, 0.0, 0.0));

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


    btTransform ModeltransIdentity;
    ModeltransIdentity.setIdentity();
    const btVector3 Modelcolor(1.0f, 1.0f, 1.0);
    
    TreeNode<int> test_rootNode; test_rootNode.getData() = 80085; //still looks cursed

    TreeNode<TreeNode_Data> rootNode;
    rootNode.setName("Root");
    rootNode.getData().mShape = new btCompoundShape();//std::make_unique<btCompoundShape>();
    rootNode.getData().mType = lpt::BulletShapeType::CompoundShape;
    rootNode.getData().not_loaded = false;

    double lastFrameTime = glfwGetTime();
    while(!window.shouldClose())
    {
        window.pollEvents();

        const double deltaTime = glfwGetTime() - lastFrameTime;
        lastFrameTime = glfwGetTime();

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
            ImGui::End();
        }

        /*

        if (0)
        if(ImGui::Begin("Create model"))
        {
            static std::size_t val_previously_selected = 0;
            const char* val_preview = gCollisionShapeTypesNames[val_previously_selected];
            if(ImGui::BeginCombo("btCollisionShape", val_preview))
            {
                for(std::size_t i = 0; i < static_cast<std::size_t>(lpt::BulletShapeType::Size); ++i)
                {
                    const bool is_selected = (val_previously_selected == i);
                    if(ImGui::Selectable(gCollisionShapeTypesNames[i], is_selected))
                    {
                        val_previously_selected = i;
                    }
                }
                ImGui::EndCombo();
            }
            // 0 - radius ; boxHalfExtents.X ; radius
            // 1 -        ; boxHalfExtents.Y ; height
            // 2 -        ; boxHalfExtents.Z ;
            static float var_float[3] = {};

            switch(static_cast<lpt::BulletShapeType>(val_previously_selected))
                {
                    case lpt::BulletShapeType::CompoundShape:
                    {
                        
                        break;
                    }

                    case lpt::BulletShapeType::CapsuleShapeY:
                    [[fallthrough]];
                    case lpt::BulletShapeType::CapsuleShapeX:
                    [[fallthrough]];
                    case lpt::BulletShapeType::CapsuleShapeZ:
                    [[fallthrough]];
                    case lpt::BulletShapeType::ConeShapeY:
                    [[fallthrough]];
                    case lpt::BulletShapeType::ConeShapeX:
                    [[fallthrough]];
                    case lpt::BulletShapeType::ConeShapeZ:
                    {
                        ImGui::SliderFloat("radius", &var_float[0], 0.001f, 100.0f);
                        ImGui::SliderFloat("height", &var_float[1], 0.001f, 100.0f);
                        break;
                    }

                    case lpt::BulletShapeType::CylinderShapeY:
                    [[fallthrough]];
                    case lpt::BulletShapeType::CylinderShapeX:
                    [[fallthrough]];
                    case lpt::BulletShapeType::CylinderShapeZ:
                    [[fallthrough]];
                    case lpt::BulletShapeType::BoxShape:
                    {
                        ImGui::SliderFloat3("boxHalfExtents", &var_float[0], 0.001f, 100.0f);
                        break;
                    }
                    case lpt::BulletShapeType::SphereShape:
                    {
                        ImGui::SliderFloat("radius", &var_float[0], 0.001f, 100.0f);
                        break;
                    }
                    case lpt::BulletShapeType::ConvexHullShape:
                    {
                        break;
                    }

                    case lpt::BulletShapeType::Size:
                    [[fallthrough]];
                    default:
                        std::cerr << "Quo Vadis?\n";
                        break; 
                }

            if(ImGui::Button("Create"))
            {
                
                switch(static_cast<lpt::BulletShapeType>(val_previously_selected))
                {
                    case lpt::BulletShapeType::CompoundShape:
                    {
                        break;
                    }

                    case lpt::BulletShapeType::CapsuleShapeY:
                    {
                        delete vPhysicsShape;
                        vPhysicsShape = new btCapsuleShape(var_float[0], var_float[1]);
                        break;
                    }
                    case lpt::BulletShapeType::CapsuleShapeX:
                    {
                        delete vPhysicsShape;
                        vPhysicsShape = new btCapsuleShapeX(var_float[0], var_float[1]);
                        break;
                    }
                    case lpt::BulletShapeType::CapsuleShapeZ:
                    {
                        delete vPhysicsShape;
                        vPhysicsShape = new btCapsuleShapeZ(var_float[0], var_float[1]);
                        break;
                    }

                    case lpt::BulletShapeType::ConeShapeY:
                    {
                        delete vPhysicsShape;
                        vPhysicsShape = new btConeShape(var_float[0], var_float[1]);
                        break;
                    }
                    case lpt::BulletShapeType::ConeShapeX:
                    {
                        delete vPhysicsShape;
                        vPhysicsShape = new btConeShapeX(var_float[0], var_float[1]);
                        break;
                    }
                    case lpt::BulletShapeType::ConeShapeZ:
                    {
                        delete vPhysicsShape;
                        vPhysicsShape = new btConeShapeZ(var_float[0], var_float[1]);
                        break;
                    }

                    case lpt::BulletShapeType::CylinderShapeY:
                    {
                        delete vPhysicsShape;
                        vPhysicsShape = new btCylinderShape(btVector3(var_float[0], var_float[1], var_float[2]));
                        break;
                    }
                    case lpt::BulletShapeType::CylinderShapeX:
                    {
                        delete vPhysicsShape;
                        vPhysicsShape = new btCylinderShapeX(btVector3(var_float[0], var_float[1], var_float[2]));
                        break;
                    }
                    case lpt::BulletShapeType::CylinderShapeZ:
                    {
                        delete vPhysicsShape;
                        vPhysicsShape = new btCylinderShapeZ(btVector3(var_float[0], var_float[1], var_float[2]));
                        break;
                    }

                    case lpt::BulletShapeType::BoxShape:
                    {
                        delete vPhysicsShape;
                        vPhysicsShape = new btBoxShape(btVector3(var_float[0], var_float[1], var_float[2]));
                        break;
                    }

                    case lpt::BulletShapeType::SphereShape:
                    {
                        delete vPhysicsShape;
                        vPhysicsShape = new btSphereShape(var_float[0]);
                        break;
                    }

                    case lpt::BulletShapeType::ConvexHullShape:
                    {
                        break;
                    }

                    case lpt::BulletShapeType::Size:
                    [[fallthrough]];
                    default:
                        std::cerr << "Queo Vadis?\n";
                        break; 
                }
                vPhysicsBody->setCollisionShape(vPhysicsShape);
            }
           
            
            ImGui::End();
        }

        */

        ImGui::ShowDemoWindow();

        if(ImGui::Begin("Test"))
        {
            //ImGui_DrawTreeNode(test_rootNode);

            ImGui_DrawTreeNode(rootNode);

            
            ImGui::End();
        }

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

        // make a menu to open/close all the windows
        // make a menu to load a model & display it
        // make a menu to import/export files

        mPlayer.update(deltaTime);
        dynamicsWorld->debugDrawWorld();
        dynamicsWorld->stepSimulation(deltaTime);

        glClipControl(GL_LOWER_LEFT, GL_ZERO_TO_ONE); //set clipping plane to [0,1], instead of the default [-1,1] 
        glDepthFunc(GL_GREATER); //these 3 reverse the depth buffer
        glClearDepth(0); //clear depth to 0
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f); //sets the clear colour to black (red)
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        if(!bulletDebugRenderer.verticies.empty())
        {
            GLuint VBO = 0;
            glCreateBuffers(1, &VBO);
            glNamedBufferStorage(VBO, bulletDebugRenderer.verticies.size() * sizeof(lp::gl::Bullet3Debug::Vertex), bulletDebugRenderer.verticies.data(), 0);
            glVertexArrayVertexBuffer(VAO_model, 0, VBO, 0, 6 * sizeof(float));

            lp::gl::RegularShader shader;
            shader.LoadShader(lp::gl::ShaderType::DebugLine);
            shader.Use();
            shader.SetUniform(1, mPlayer.getViewMatrix());

            {
                int width = 0, height = 0;
                window.debugGetFramebufferSize(width, height);
                glViewport(0, 0, width, height);
                shader.SetUniform(2, mPlayer.getProjectionMatrix((double)width / (double)height));
            }
            shader.SetUniform(3, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f)));

            glBindVertexArray(VAO_model);
            glDrawArrays(GL_LINES, 0, bulletDebugRenderer.drawCount);
            glBindVertexArray(0);
            glDeleteBuffers(1, &VBO);
        }

       
        window.swapBuffers();
    }

    lp::g_engine.destroy();
    window.destroy();

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
}

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