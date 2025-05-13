#include "Labyrinth/Engine/Graphics/LightSystem.hpp"

#include "Labyrinth/engine.hpp"

#include <iostream>
#include <algorithm> //std::find

namespace lp
{
    void LightSystem::update()
    {
        if(this->mDirty)
        {
            if(this->mEntities.size() > mLightCount)
            {
                this-> updateIncreaseLights(); //adding lights is required
            } else if(this->mEntities.size() < mLightCount)
            {
                //removing lights is required
                for(auto entIter = mLightBufferEntityMap.begin(); entIter != mLightBufferEntityMap.end(); ++entIter)
                {
                    if(*entIter == lp::ecs::const_entity_invalid) continue;
                    const lp::ecs::Entity eent = *entIter;
                    const auto iter = std::find(mEntities.begin(), mEntities.end(), eent);
                    if(iter == mEntities.end())
                    {
                        (*entIter) = lp::ecs::const_entity_invalid; //remove light
                        mLightCount--;
                    }
                }

                if(mMaxLightCount > mLightCount * 2.1) //if there are too many empty spaces for lights
                {
                    this->updateGarbageCollection(); //remove a couple of them, freeing space
                }

            } else //both numbers are equal
            {
                // mDirty updates only on changes in mEntities size
                // so this should only get called on initialization
                if(mShaderStorageBufferLights == 0)
                {
                    glCreateBuffers(1, &mShaderStorageBufferLights);
                    mMaxLightCount = 8; //randomly chosen default value
                    glBufferStorage(mShaderStorageBufferLights, sizeof(lp::ComponentLight::StructuredGLSL) * mMaxLightCount, nullptr, GL_DYNAMIC_STORAGE_BIT);

                    glCreateBuffers(1, &mAliveLightListBuffer);
                    glBufferStorage(mShaderStorageBufferLights, sizeof(GLuint) * mMaxLightCount, nullptr, GL_DYNAMIC_STORAGE_BIT);
                }
            }

            //update the list od living lights
            mAliveLightList.clear(); mAliveLightList.reserve(mLightBufferEntityMap.size()); //these 2 should be of equal size at the end of this
            //for(const auto ent:mLightBufferEntityMap)
            for(std::size_t i = 0; i < mLightBufferEntityMap.size(); ++i)
            {
                if(mLightBufferEntityMap[i] != lp::ecs::const_entity_invalid)
                {
                    mAliveLightList.push_back(static_cast<GLuint>(i));
                }
            }
            //update the buffer too, assume that the data will fit
            glNamedBufferSubData(mAliveLightListBuffer, 0, sizeof(GLuint) * mAliveLightList.size(), mAliveLightList.data());

        } else 
        {
            this->updateNotDirty();
        }
    }

    void LightSystem::updateGarbageCollection()
    {
        mMaxLightCount = mMaxLightCount / 2; //decrease capacity by half
        if(mLightCount > mMaxLightCount) mMaxLightCount = mLightCount + 1; //just in case

        if(mShaderStorageBufferLights) glDeleteBuffers(1, &mShaderStorageBufferLights);
        if(mAliveLightListBuffer) glDeleteBuffers(1, &mAliveLightListBuffer);
        glCreateBuffers(1, &mShaderStorageBufferLights);
        glCreateBuffers(1, &mAliveLightListBuffer);
        glBufferStorage(mShaderStorageBufferLights, sizeof(lp::ComponentLight::StructuredGLSL) * mMaxLightCount, nullptr, GL_DYNAMIC_STORAGE_BIT);
        glBufferStorage(mAliveLightListBuffer, sizeof(GLuint) * mMaxLightCount, nullptr, GL_DYNAMIC_STORAGE_BIT);

        mLightBufferEntityMap.resize(mMaxLightCount, lp::ecs::const_entity_invalid); //make map smaller

        assert(mLightBufferEntityMap.size() == mEntities.size());

        auto& ecs = lp::g_engine.getECS();

        std::size_t i = 0;
        for(const auto ent: this->mEntities)
        {
            mLightBufferEntityMap[i] = ent;
            this->updateSingleLight(i * sizeof(lp::ComponentLight::StructuredGLSL), ecs.getComponent<ComponentLight>(ent));
            ++i;
        }
    }

    void LightSystem::updateIncreaseLights()
    {
        std::vector<lp::ecs::Entity> entities_to_add; entities_to_add.reserve(4); //create & reserve some storage for vector of missing Entities
        for(const auto ent: this->mEntities)
        {
            const auto iter = std::find(mLightBufferEntityMap.begin(), mLightBufferEntityMap.end(), ent);
            if(iter == mLightBufferEntityMap.end())
            {
                if(mLightCount < mMaxLightCount)
                {
                    //we have space, add the light now
                    this->addLightSingle(ent);
                } else
                {
                    //we have run out of space, resize the buffer THEN add them back in
                    entities_to_add.push_back(ent); 
                }
            }
        }
        if(entities_to_add.size() <= 0) return; //all lights added, early return

        //if no space left for lights, make buffers bigger

        GLuint tempBufferLights = 0;
        GLuint tempBufferAlive = 0;
        glCreateBuffers(1, &tempBufferLights);
        glCreateBuffers(1, &tempBufferAlive);

        std::size_t tempMaxLightCount = mMaxLightCount * 2; //increase capacity by 2

        glBufferStorage(tempBufferLights, sizeof(lp::ComponentLight::StructuredGLSL) * tempMaxLightCount, nullptr, GL_DYNAMIC_STORAGE_BIT);
        glBufferStorage(tempBufferAlive, sizeof(GLuint) * tempMaxLightCount, nullptr, GL_DYNAMIC_STORAGE_BIT);
        if(mShaderStorageBufferLights)
        {
            glCopyNamedBufferSubData(mShaderStorageBufferLights, tempBufferLights, 0, 0, sizeof(lp::ComponentLight::StructuredGLSL) * mMaxLightCount);
            std::cerr << "LightSystem::update(): undefined behaviour @ line" << __LINE__ << "\n";
        }
        if(mAliveLightListBuffer)
        {
            glCopyNamedBufferSubData(mAliveLightListBuffer, tempBufferAlive, 0, 0, sizeof(GLuint) * mMaxLightCount);
            std::cerr << "LightSystem::update(): undefined behaviour @ line" << __LINE__ << "\n";
        }
                    
        mMaxLightCount = tempMaxLightCount;

        if(mShaderStorageBufferLights) glDeleteBuffers(1, &mShaderStorageBufferLights);
        if(mAliveLightListBuffer) glDeleteBuffers(1, &mAliveLightListBuffer);
        mShaderStorageBufferLights = tempBufferLights;
        mAliveLightListBuffer = tempBufferAlive;

        mLightBufferEntityMap.resize(tempMaxLightCount, lp::ecs::const_entity_invalid); //add space to mLightBufferEntityMap (fills with const_entity_invalid)
                    
        //now that there is space, add the missing lights
                    
        for(const auto ent: entities_to_add)
        {
            this->addLightSingle(ent);
        }
    }

    void LightSystem::updateNotDirty()
    {
        auto& ecs = lp::g_engine.getECS();
        for(const auto&ent: this->mEntities)
        {
            auto& rlight = ecs.getComponent<ComponentLight>(ent);
            if(rlight.getDirty())
            {
                const auto iter = std::find(mLightBufferEntityMap.begin(), mLightBufferEntityMap.end(), ent);
                if(iter != mLightBufferEntityMap.end()) [[likely]]
                {
                    const std::size_t index = std::distance(mLightBufferEntityMap.begin(), iter);
                    this->updateSingleLight(index * sizeof(lp::ComponentLight::StructuredGLSL), rlight);
                } else [[unlikely]]
                {
                    std::cerr << "LightSystem::updateNotDirty(): an existing light does not exist in mLightBufferEntityMap!\n";
                }
            }
        }
    }

    void LightSystem::addLightSingle(const lp::ecs::Entity cv_lightEnt)
    {
        auto iter = std::find(mLightBufferEntityMap.begin(), mLightBufferEntityMap.end(), lp::ecs::const_entity_invalid);
        if(iter == mLightBufferEntityMap.end()) [[unlikely]]
        {
            std::cerr << "LightSystem::addLightSingle() used when there is no space!";
            return;
        }
        *iter = cv_lightEnt;
        const std::size_t index = std::distance(mLightBufferEntityMap.begin(), iter);
        this->updateSingleLight(index * sizeof(lp::ComponentLight::StructuredGLSL), lp::g_engine.getECS().getComponent<ComponentLight>(cv_lightEnt));
        ++mLightCount;
    }

    void LightSystem::updateSingleLight(const GLintptr cv_offset, lp::ComponentLight& r_light)
    {
        lp::ComponentLight::StructuredGLSL dstruk;
        r_light.getGLSLStructure(dstruk);
        //TODO: add shadow dstruk.shadowID = setting code here!
        glNamedBufferSubData(mShaderStorageBufferLights, cv_offset, sizeof(dstruk), &dstruk);
    }
}