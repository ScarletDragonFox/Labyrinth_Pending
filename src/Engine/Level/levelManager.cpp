#include "Labyrinth/Engine/Level/levelManager.hpp"

#include <nlohmann/json.hpp>
#include <fstream>

#include "Labyrinth/Helpers/compilerErrors.hpp"

LP_PRAGMA_DISABLE_ALL_WARNINGS_PUSH();
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <bullet/Extras/Serialize/BulletWorldImporter/btBulletWorldImporter.h>

LP_PRAGMA_DISABLE_ALL_WARNINGS_POP();

#include "Labyrinth/Engine/ComponentLight.hpp"
#include "Labyrinth/Engine/ComponentPosition.hpp"
#include "Labyrinth/Engine/componentModel.hpp"
#include "Labyrinth/Engine/ComponentPhysics.hpp"
// #include "Labyrinth/Engine/ComponentSoundSource.hpp"

#include <Labyrinth/engine.hpp>
#include <Labyrinth/Engine/Resource/resourceManager.hpp>
#include <Labyrinth/Engine/ECS/coreECS.hpp>

using json = nlohmann::json;

namespace glm
{
    void to_json(json& j, const glm::vec3& v)
    {
        j = json{{"x", v.x}, {"y", v.y},{"z", v.z}};
    }

    void from_json(const  json& j, glm::vec3& v)
    {
        j.at("x").get_to(v.x);
        j.at("y").get_to(v.y);
        j.at("z").get_to(v.z);
    }

    void to_json(json& j, const glm::quat& v)
    {
        j = json{{"x", v.x}, {"y", v.y}, {"z", v.z}, {"w", v.w}};
    }

    void from_json(const  json& j, glm::quat& v)
    {
        j.at("x").get_to(v.x);
        j.at("y").get_to(v.y);
        j.at("z").get_to(v.z);
        j.at("w").get_to(v.w);
    }
}



namespace lp
{
    struct ModelSerialize
    {
        struct Instance
        {
            glm::vec3 position = {};
            glm::vec3 scale = {};
            glm::quat rotation = {};
            NLOHMANN_DEFINE_TYPE_INTRUSIVE(Instance, position, scale, rotation)
        };
        std::vector<Instance> instances;
        /// @brief name/filepath of the (.obj) model file
        std::string model_filename;
        /// @brief name/filepath of the .bullet physics file
        std::string collision_filename;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(ModelSerialize, model_filename, collision_filename, instances)
    };

    // struct CollisionAsset
    // {
    //     enum class CollisionType
    //     {
    //         /// @brief mass > 0, fully simulated by bullet
    //         DYNAMIC,
    //         /// @brief mass of 0, can't / shouldn't be moved
    //         STATIC,
    //         /// @brief mass of 0, can be moved, don't collide with DYNAMIC or STATIC objects
    //         KINEMATIC
    //     };
    //     CollisionType collision_type = CollisionType::DYNAMIC;
    //     float mass = 1.0;
    // };
    // NLOHMANN_JSON_SERIALIZE_ENUM(CollisionAsset::CollisionType, {
    //     {CollisionAsset::CollisionType::DYNAMIC, "dynamic"},
    //     {CollisionAsset::CollisionType::STATIC, "static"},
    //     {CollisionAsset::CollisionType::KINEMATIC, "kinematic"}
    // })
    // NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(CollisionAsset, collision_type, mass)

    struct ModelAsset
    {
        std::string graphics_filepath;
        std::string collision_filepath;
    };

        void to_json(json& j, const lp::ComponentLight& l)
        {
            j = json{{"position", l.getPosition()}, {"color", l.getColor()}, {"radius", l.getRadius()}};
        }

        void from_json(const json& j, lp::ComponentLight& l) 
        {
            glm::vec3 pos = {};
            j.at("position").get_to(pos);
            l.setPosition(pos); pos = {};
            j.at("color").get_to(pos);
            l.setColor(pos);
            float rad = 0.0;
            j.at("radius").get_to(rad);
            l.setRadius(rad);
        } 
    

    void LevelManager::loadLevel(std::string_view name)
    {
        std::ifstream file(name.data());
        if(!file.is_open()) return;

        nlohmann::json data = nlohmann::json::parse(file);

        std::vector<lp::ComponentLight> loaded_lights;

        if(data.contains("lights"))
        {
            for(const auto& dat: data["lights"])
            {
                try
                {
                    loaded_lights.push_back(dat.get<lp::ComponentLight>());
                }catch(const json::exception& e)
                {
                    std::cerr << "LevelManager::JSON::LIGHTS: " << e.what() << "\n";
                }
            }
        } else std::cout << "LevelManager::JSON: no \"lights\"!\n";

        std::vector<ModelSerialize> loaded_models;

        if(data.contains("models"))
        {
            for(const auto& dat: data["models"])
            {
                try
                {
                    loaded_models.push_back(dat.get<ModelSerialize>());
                }catch(const json::exception& e)
                {
                    std::cerr << "LevelManager::JSON::LIGHTS: " << e.what() << "\n";
                }
            }
        } else std::cout << "LevelManager::JSON: no \"models\"!\n";
        // std::cout << "lp::ComponentLight count: "<< loaded_lights.size() <<"\n";
        // std::cout << "ModelSerialize count: "<< loaded_models.size() <<"\n";


        auto& ResMan = lp::g_engine.getResurceManager();
        auto& Recs = lp::g_engine.getECS();

        lp::LevelManager::Level lvl;

        auto& RphysicsWorld = lp::g_engine.getPhysicsWorld();

        for(const auto&i:loaded_models)
        {
            lp::res::ModelID_t mdlID = ResMan.loadModel(i.model_filename);
            lp::ph::ColliderID_t collider = lp::ph::const_collider__id_invalid;
            btRigidBody* rbp = nullptr;
            btCollisionShape* collshape = nullptr;
            if(i.collision_filename.size() > 3)
            {
                btBulletWorldImporter* fileLoader = new btBulletWorldImporter( RphysicsWorld.getWorld());
                if(!fileLoader->loadFile(i.collision_filename.data()))
                {
                    std::cerr << "LevelManager::PHYSICS: \"" << i.collision_filename <<"\" couldn't be loaded!\n";
                    delete fileLoader;
                    continue;
                }
                if(fileLoader->getNumCollisionShapes() <= 0)
                {
                    delete fileLoader;
                    continue;
                }
                if(fileLoader->getNumCollisionShapes() > 1)
                {
                    std::cerr << "LevelManager::PHYSICS: \"" << i.collision_filename <<"\" had "<< fileLoader->getNumCollisionShapes()  <<" collision shapes (more from 1)\n";
                }
                collshape = fileLoader->getCollisionShapeByIndex(0);
                if(fileLoader->getNumRigidBodies() > 0)
                {
                   rbp = dynamic_cast<btRigidBody*>(fileLoader->getRigidBodyByIndex(0));
                }
                collider = RphysicsWorld.registerCollisionShape(collshape);
                if(collider == lp::ph::const_collider__id_invalid)
                {
                    std::cerr << "LevelManager::PHYSICS: \"" << i.collision_filename <<"\" caouldn't be registered in getPhysicsWorld()!\n";
                    continue;
                }
                //load collision from .bullet file (create a class)???
                //Also, all of this should be in its own file.
                //For the .bullet deserialization, we need a way to .bullet -> vector of btCollisionShapes
                // than create a btCompositionShape if necessary (if count > 1)
                // store all of them in a vector?
                // and for unloading first do btCompositionShape, than all the other ones
                
                lvl.mCollision.push_back(collider);
                delete fileLoader;
            }
            lvl.mModels.push_back(mdlID);
            for(const auto&j:i.instances)
            {
                lp::ecs::Entity entity = Recs.createEntity();
                lp::ComponentModel lm;
                lm.mID = mdlID;
                Recs.addComponent(entity, lm);
                if(collider == lp::ph::const_collider__id_invalid)
                {
                    lp::ComponentPosition cp;
                    cp.setPosition(j.position);
                    cp.setRotation(j.rotation);
                    cp.setScale(j.scale);
                    Recs.addComponent(entity, cp);
                } else
                {
                    lp::ComponentPhysics phc;
                    phc.mState = std::make_shared<btDefaultMotionState>();
                    if(rbp != nullptr)
                    {
                        phc.mRigidBody = std::shared_ptr<btRigidBody>(rbp);
                        btTransform& transportTransform = rbp->getWorldTransform();
                        phc.mState->setWorldTransform(transportTransform);
                    } else 
                    {
                        btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, phc.mState.get(), collshape);
                        phc.mRigidBody = std::make_shared<btRigidBody>(groundRigidBodyCI);
                    }
                    phc.setPosition(j.position);
                    phc.setRotation(j.rotation);
                    Recs.addComponent(entity, phc);
                }
                
                
                lvl.mModelsEntities.push_back(entity);
            }
            
            lvl.mCollisionOriginID = mNextCollisionOriginID;
            this->mLevels[std::string(name)] = lvl;
            ++mNextCollisionOriginID;
        }

        for(const auto&i:loaded_lights)
        {
            lp::ecs::Entity entity = Recs.createEntity();
            Recs.addComponent(entity, i);
            lvl.mLights.push_back(entity);
        }
        
        file.close();
    }

    void LevelManager::unloadLevel(std::string_view name)
    {
        if(!this->mLevels.contains(std::string(name))) return;
        auto& lvl = this->mLevels[std::string(name)];
        auto& Rman = lp::g_engine.getResurceManager();
        auto& Recs = lp::g_engine.getECS();
        auto& RphysicsWorld = lp::g_engine.getPhysicsWorld();
        for(const auto&i:lvl.mLights)
        {
            Recs.destroyEntity(i);
        }
        for(const auto&i:lvl.mModelsEntities)
        {
            Recs.destroyEntity(i);
        }
        for(const auto&i:lvl.mModels)
        {
            Rman.getModelLoaderRef().unload(i);
        }
        for(const auto&i:lvl.mCollision)
        {
            RphysicsWorld.unloadCollisionShape(i);
        }
        
    }
}