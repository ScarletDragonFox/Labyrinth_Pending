#ifndef LABYRINTH_PENDING_ENGINE_ECS_SYSTEMMANAGER_HPP
#define LABYRINTH_PENDING_ENGINE_ECS_SYSTEMMANAGER_HPP

#include "types.hpp"
#include "baseSystem.hpp"

#include <memory> //for std::shared_ptr
#include <cassert>
#include <unordered_map>

namespace lp::ecs
{
    /// @brief The System Manager. Oversees BaseSystem's
    ///
    /// This class manages:
    /// 
    ///  - signatures of systems (automatic assignment)
    /// 
    ///  - destroying Entities (removing the mfrom the std::set of all systems)
    ///
    ///  - changing entity signatures (removing from and adding the Entity to the right systems)
    class SystemManager
    {
        public:
        /// @brief register a new system
        /// @tparam T class type derived from System
        /// @param cv_systemSignature Signature of the system - what Component does it want all its Entities to have
        /// @return shared pointer to system
        /// @throw std::bad_alloc - when std::make_shared() failes
        template<typename T>
        inline std::shared_ptr<T> registerSystem(const Signature cv_systemSignature)
        {
            const char* systemName = typeid(T).name();
            assert(mSystems.find(systemName) == mSystems.end() && "Registering system more than once!");
            auto system = std::make_shared<T>();
            mSystems.insert({systemName, system});
            mSystemSignatures.insert({systemName, cv_systemSignature});
            return system;
        }   

        /// @brief get an existing system
        /// @tparam T class type derived from System
        /// @return shared pointer to system OR nullptr+assert if not found
        template<typename T>
        inline std::shared_ptr<T> getSystem()
        {
            const char* systemName = typeid(T).name();
            auto sys = mSystems.find(systemName);
            if(sys == mSystems.end())
            {
                assert(mSystems.find(systemName) != mSystems.end() && "Getting a system that wasn't registered!");
                return nullptr; //here just in case
            }

            return std::dynamic_pointer_cast<T>(sys->second);
        }

        /// @brief get the signature of a component
        /// @tparam T type of component
        /// @return signature of the component (or assert)
        template<typename T>
        inline Signature getSignature()const
        {
            const char* systemName = typeid(T).name();
            assert(mSystems.find(systemName) != mSystems.end() && "Using an unregistered system!");
            return mSystemSignatures.at(systemName);
        }

        /// @brief destory an entity. 
        ///
        /// Removes it from all systems immediately
        /// @param cv_entity 
        void destroyEntity(const Entity cv_entity);

        /// @brief called when an entities signature was changed. (and the entity needs to have new systems assigned)
        ///
        /// Updates the systems entity lists to account for the new signature of the entity
        /// @param cv_entity entity in question
        /// @param cv_signature new signature
        void entitySignatureChanged(const Entity cv_entity, const Signature cv_signature);
        
        private:
        /// @brief map of System (name) to Signature
        std::unordered_map<const char*, Signature> mSystemSignatures; 
        /// @brief map of System (name) to pointer
        std::unordered_map<const char*, std::shared_ptr<System>> mSystems;
    };  
}

#endif //LABYRINTH_PENDING_ENGINE_ECS_SYSTEMMANAGER_HPP