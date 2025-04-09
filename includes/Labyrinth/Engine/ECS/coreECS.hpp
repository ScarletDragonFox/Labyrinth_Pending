#ifndef LABYRINTH_PENDING_ENGINE_ECS_COREECS_HPP
#define LABYRINTH_PENDING_ENGINE_ECS_COREECS_HPP

#include "entityManager.hpp"
#include "systemManager.hpp"
#include "componentManager.hpp"

namespace lp
{
    /// @brief The Core/Main ECS class.
    ///
    /// This is the main (and only) interface of the ECS system.
    ///
    /// This deals with everything the ECS does.
    ///
    /// Stores the individual ECS managers in itself.
    ///
    /// Not multithreading-safe in the slightest!!!
    class CoreECS
    {
        public:

        /// @brief create a new Entity
        /// @return Entity id
        Entity createEntity();

        /// @brief destroys an entity
        /// @param cv_entity entity id to destroy
        /// @note may or may not crash if id is invalid. THIS SETS TRUE ALL SYSTEMS DIRTY FLAGS!
        void destroyEntity(const Entity cv_entity);

        /// @brief checks if a given entity is still alive
        /// @param cv_entity the entity id to check
        /// @return true for alive, false for not
        /// @warning Can be resource-intesive! uses std::find!
        bool isAlive(const Entity cv_entity)const;

        /// @brief register a new Component, with a max of 16
        /// @tparam T the type of the Component to register
        template<typename T>
        inline void registerComponent()
        {
            mComponent.registerComponent<T>();
        }

        /// @brief add (attach) a Component to an Entity
        ///
        /// type doesn't need to be specifies in this overload
        /// @tparam T type of Component to attach
        /// @param cv_entity Entity in question
        /// @param r_component reference to component to add
        template<typename T>
        inline void addComponent(const Entity cv_entity, const T& r_component)
        {
            mComponent.addComponent(cv_entity, r_component);
            Signature& entitySign = mEntity.getSignature(cv_entity);
            Signature componentSign = mComponent.getComponentSignature<T>();
            entitySign |= componentSign;
            mSystem.entitySignatureChanged(cv_entity, entitySign);
        }

        /// @brief add (attach) a Component to an Entity
        ///
        /// Component will have its defualt value, as obtained by it constructor
        /// @tparam T type of Component to attach 
        /// @param cv_entity Entity in question
        template<typename T>
        inline void addComponent(const Entity cv_entity)
        {
            mComponent.addComponent(cv_entity);
            Signature& entitySign = mEntity.getSignature(cv_entity);
            Signature componentSign = mComponent.getComponentSignature<T>();
            entitySign |= componentSign;
            mSystem.entitySignatureChanged(cv_entity, entitySign);
        }

        /// @brief Remove a Component from an Entity
        ///
        /// Does nothing if the Entity doesn't have that Component
        /// @tparam T type of Component to remove 
        /// @param cv_entity Entity in question
        template<typename T>
        inline void removeComponent(const Entity cv_entity)
        {
            mComponent.removeComponent<T>(cv_entity);
            Signature& entitySign = mEntity.getSignature(cv_entity);
            Signature componentSign = mComponent.getComponentSignature<T>();
            entitySign &= ~componentSign;
            mSystem.entitySignatureChanged(cv_entity, entitySign);
        }

        /// @brief Obtain a reference to a type T Component of specified Entity
        /// @tparam T type of Component to retrive
        /// @param cv_entity Entity in question
        /// @return reference to Component
        /// @warning This WILL crash if the Entity doesn't have that Component!!
        template<typename T>
        inline T& getComponent(const Entity cv_entity)
        {
            return mComponent.getComponent<T>(cv_entity);
        }

        /// @brief get the Signature of an Component.
        /// @tparam T type id of the Component
        /// @return Signature (bit-field with 1 bit set) of this Component
        /// @warning This WILL crash if the Entity doesn't have that Component!!
        template<typename T>
        inline Signature getComponentSignature()const
        {
            return mComponent.getComponentSignature<T>();
        }

        /// @brief returns whether this Entity has this component
        /// @tparam T 
        /// @param cv_entity 
        /// @return true for yes, false otherwise
        template<typename T>
        inline bool hasComponent(const Entity cv_entity)
        {
            const Signature entSign = mEntity.getSignature(cv_entity);
            const Signature compSign = mComponent.getComponentSignature<T>();
            return (entSign & compSign) == compSign;
        }

        /// @brief register a new system
        /// @tparam T class type derived from baseSystem
        /// @param cv_systemSignature Signature of the system - what Component does it want all its Entities to have
        /// @return shared pointer to system
        /// @throw std::bad_alloc - when std::make_shared() failes
        template<typename T>
        inline std::shared_ptr<T> registerSystem(const Signature cv_systemSignature)
        {
            return mSystem.registerSystem<T>(cv_systemSignature); //automatic signature!!!!!
        }

        private:
        /// @brief The Manager of Systems
        SystemManager mSystem;
        /// @brief The Manager of Entities
        EntityManager mEntity;
        /// @brief The Manager of Components
        ComponentManager mComponent;
    };
}

#endif //LABYRINTH_PENDING_ENGINE_ECS_COREECS_HPP