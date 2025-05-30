#include "Labyrinth/Engine/Resource/soundManager.hpp"

#include "Labyrinth/engine.hpp"
#include "Labyrinth/Engine/ECS/coreECS.hpp"

#include "Labyrinth/Engine/ComponentSoundSource.hpp"
#include "Labyrinth/Engine/ComponentPhysics.hpp"
#include "Labyrinth/Engine/ComponentPosition.hpp"

#include <vector>
#include <iostream>

#include <fstream>
#include <nlohmann/json.hpp>
#include <soloud_wav.h>
#include <soloud.h>

#include <cmath>

namespace lp::res
{
    bool SoundManager::initialize(const std::string_view soundsfilename)
    {
        {
            auto& Recs = g_engine.getECS();
            const lp::ecs::Signature SoundCompSign = Recs.getComponentSignature<ComponentSoundSource>();
            const lp::ecs::Signature PhysicsCompSign = Recs.getComponentSignature<ComponentPhysics>();
            const lp::ecs::Signature PositionCompSign = Recs.getComponentSignature<ComponentPosition>();

            Recs.registerSystem<SoundManager_AuxillaryPhysics>(SoundCompSign | PhysicsCompSign);
            Recs.registerSystem<SoundManager_AuxillaryPosition>(SoundCompSign | PositionCompSign);
        }
        

        using json = nlohmann::json;
        std::ifstream file(soundsfilename.data());
        if(!file.is_open()) return true;

        json data = json::parse(file);
        if(!data.contains("array"))
        {
            return true;
        }
        for(const auto& dat: data["array"])
        {
            try
            {
                if(dat.contains("filename"))
                {
                    std::string filename = dat["filename"].get<std::string>();
                    if(mAudios.contains(filename)) continue;
                    SoLoud::Wav *wavp = new SoLoud::Wav();
                    int res = wavp->load(filename.c_str());
                    if(res == 0)
                    {
                        AudioData ad;
                        ad.source = wavp;
                        mAudios[filename] = ad;
                    } else{
                        std::cerr << "Could't load " << filename << " SoLoud's result: " << res << "\n";
                        delete wavp;
                    }
                }
            } catch(const json::exception& e)
            {
                std::cerr << "SoundManager::JSON: " << e.what() << "\n";
            }
        }
        //std::cout << data.dump() << "\n";

        file.close();
        return false;
    }

    SoundManager::~SoundManager()
    {
        this->stopAll();
        for(const auto& as:mAudios)
        {
            delete as.second.source;
        }
    }

    bool SoundManager::isLoaded(const std::string_view soundNameID)
    {
        return mAudios.find(soundNameID.data()) != mAudios.end();
    }

    void SoundManager::playSoundEntity(const lp::ecs::Entity cv_entity, const std::string_view soundNameID)
    {
        auto iter = mAudios.find(soundNameID.data());
        if(iter != mAudios.end())
        {
            auto& ecsR = lp::g_engine.getECS();
            if(!ecsR.isAlive(cv_entity))
            {
                std::cerr << "Entity (" << cv_entity << ") tried to playSoundEntity() whilest dead!\n";
                return;
            }
            if(ecsR.hasComponent<ComponentSoundSource>(cv_entity))
            {
                std::cerr << "Entity (" << cv_entity << ") tried to playSoundEntity() while previous playing!\n";
                return;
            }

            auto& sL = lp::g_engine.getSoLoud();
            glm::vec3 pos = {};
            glm::vec3 velo = {};
            if(ecsR.hasComponent<lp::ComponentPhysics>(cv_entity))
            {
                auto& physicsC = ecsR.getComponent<lp::ComponentPhysics>(cv_entity);
                pos = physicsC.getPosition();
                velo = physicsC.getVelocity();
            } else if(ecsR.hasComponent<lp::ComponentPosition>(cv_entity))
            {
                auto& positionC = ecsR.getComponent<lp::ComponentPosition>(cv_entity);
                pos = positionC.getPosition();
            }
            ComponentSoundSource CSS;
            CSS.mHandle = sL.play3d((*iter->second.source), pos.x, pos.y, pos.z, velo.x, velo.y, velo.z, 1);
            CSS.setAttenuationModel(SoLoud::AudioSource::ATTENUATION_MODELS::LINEAR_DISTANCE);
            CSS.setRolloffFactor(1.0);
            CSS.setMax(200.0);
            CSS.setMin(50.0);
            
            ecsR.addComponent(cv_entity, CSS);
        }
    }

    void SoundManager::update()
    {
        if(mGlobalStop) return;
        auto& sL = lp::g_engine.getSoLoud();
        auto& ecsR = lp::g_engine.getECS();
        
        {
            auto sm_aph = ecsR.getSystem<SoundManager_AuxillaryPhysics>();

            for(lp::ecs::Entity ent: sm_aph->getSet())
            {
                auto& soundC = ecsR.getComponent<lp::ComponentSoundSource>(ent);
                auto& physicsC = ecsR.getComponent<lp::ComponentPhysics>(ent);
                soundC.setPositionVelocity(physicsC.getPosition(), physicsC.getVelocity());
            }
        }
        {
            auto sm_apo = ecsR.getSystem<SoundManager_AuxillaryPosition>();

            for(lp::ecs::Entity ent: sm_apo->getSet())
            {
                auto& soundC = ecsR.getComponent<lp::ComponentSoundSource>(ent);
                auto& positionC = ecsR.getComponent<lp::ComponentPosition>(ent);
                soundC.setPosition(positionC.getPosition());
            }
        }
        
        std::vector <lp::ecs::Entity> entities_to_delete;

        for(const auto ent:this->mEntities)
        {
            lp::ComponentSoundSource& CSS = ecsR.getComponent<lp::ComponentSoundSource>(ent);
            if(!sL.isValidVoiceHandle(CSS.getHandle()))
            {
                entities_to_delete.push_back(ent);
            }
        }
        for(const auto ent:entities_to_delete)
        {
            ecsR.removeComponent<lp::ComponentSoundSource>(ent);
        }
    }

    void SoundManager::stopAll()
    {
       lp::g_engine.getSoLoud().stopAll();
       mGlobalStop = true;
    }
        
    void SoundManager::resumeAll()
    {
        lp::g_engine.getSoLoud().resume();
        mGlobalStop = false;
    }
}