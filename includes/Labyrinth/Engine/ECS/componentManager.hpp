#ifndef LABYRINTH_PENDING_ENGINE_ECS_COMPONENTMANAGER_HPP
#define LABYRINTH_PENDING_ENGINE_ECS_COMPONENTMANAGER_HPP

#include <memory>
#include <unordered_map>
#include <cassert>

#include "types.hpp"
#include "componentContainer.hpp"

namespace lp::ecs
{
    /// @brief class that manages the creation and destruction of Components
    ///
    /// This class manages:
    ///
    /// - storing of Components & their Signatures
    ///
    /// - registering new Components with an automatic Signature
    ///
    /// - adding & removing Components from Entities
    class ComponentManager
    {
        public:

        /// @brief register a new Component, with a max of 16
        /// @tparam T the type of the Component to register
        template<typename T>
        inline void registerComponent()
        {
            constexpr uint16_t MAX_EXPONENT = 1 << 15;

            const char* typeName = typeid(T).name();
            assert(mComponentSignatures.find(typeName) == mComponentSignatures.end() && "Registering component type more than once.");
            assert(mNextComponentIDExponent == 16 && "Reached Component Limit!");

            mComponentSignatures.insert({typeName, 1 << mNextComponentIDExponent});
            mComponentContainers.insert{typeName, std::make_shared<ComponentArray<T>>()};
            
            ++mNextComponentIDExponent;
        }
        
        /// @brief add (attach) a Component to an Entity
        ///
        /// type doesn't need to be specifies in this overload
        /// @tparam T type of Component to attach
        /// @param cv_entity Entity in question
        /// @param r_component reference to component to add
        template<typename T>
        inline void addComponent(const Entity cv_entity, T& r_component)
        {
            getComponentContainer<T>()->insert(cv_entity, r_component);
        }

        /// @brief add (attach) a Component to an Entity
        ///
        /// Component will have its defualt value, as obtained by it constructor
        /// @tparam T type of Component to attach 
        /// @param cv_entity Entity in question
        template<typename T>
        inline void addComponent(const Entity cv_entity)
        {
            getComponentContainer<T>()->insert(cv_entity, T());
        }

        /// @brief Remove a Component from an Entity
        ///
        /// Does nothing if the Entity doesn't have that Component
        /// @tparam T type of Component to remove 
        /// @param cv_entity Entity in question
        template<typename T>
        inline void removeComponent(const Entity cv_entity)
        {
            getComponentContainer<T>()->remove(cv_entity);
        }

        /// @brief Obtain a reference to a type T Component of specified Entity
        /// @tparam T type of Component to retrive
        /// @param cv_entity Entity in question
        /// @return reference to Component
        /// @warning This WILL crash if the Entity doesn't have that Component!!
        template<typename T>
        inline T& getComponent(const Entity cv_entity)
        {
            return getComponentContainer<T>()->getData(cv_entity);
        }

        /// @brief destroy an Entity, removing all its Components
        /// @param cv_entity Entity to kill Components of
        inline void destroyEntity(const Entity cv_entity)
        {
            for(auto& i: mComponentContainers)
            {
               i.second->remove(cv_entity);
            }
        }

        /// @brief get the Signature of an Component.
        /// @tparam T type id of the Component
        /// @return Signature (bit-field with 1 bit set) of this Component
        /// @warning This WILL crash if the Entity doesn't have that Component!!
        template<typename T>
        inline Signature getComponentSignature()const
        {
            const char* typeName = typeid(T).name();
            auto componentSignatureIterator = mComponentSignatures.find(typeName);
            assert(componentSignatureIterator != mComponentSignatures.end() && "Component not registered before use.");
            return componentSignatureIterator->second;
        }

        private:
        /// @brief map of Component id (name) to shared_ptr of Component Storage
        std::unordered_map<const char*, std::shared_ptr<ComponentContainerBasePureVirtualClass>> mComponentContainers;
        /// @brief map of Component id (name) to Component Signature (in which only 1 bit is set to true)
        std::unordered_map<const char*, Signature> mComponentSignatures;
        /// @brief 'exponent' of next systems signature (which bit to set)
        std::uint_fast16_t mNextComponentIDExponent = 0;
        
        /// @brief private function to return pointer to Container of specified Component
        /// @tparam T type of Component to get Storage of
        /// @return shared pointer to storage
        /// @warning This WILL crash if the Entity doesn't have that Component!!
        template<typename T>
        inline std::shared_ptr<ComponentContainer<T>> getComponentContainer()
        {
            const char* typeName = typeid(T).name();
            assert(mComponentSignatures.find(typeName) != mComponentSignatures.end() && "Component not registered before use.");
            return std::static_pointer_cast<ComponentContainer<T>>(mComponentContainers[typeName]);
        }

    };
}

#endif //LABYRINTH_PENDING_ENGINE_ECS_COMPONENTMANAGER_HPP