#ifndef LABYRINTH_PENDING_ENGINE_ECS_COMPONENTCONTAINER_HPP
#define LABYRINTH_PENDING_ENGINE_ECS_COMPONENTCONTAINER_HPP

#include "types.hpp"
#include <cassert>
#include <unordered_map>

namespace lp::ecs
{
    /// @brief Pure Virtual Class.
    ///
    /// Used as base class for actual storage to allow for diffrent Types of Component Containers (via Templates)
    class ComponentContainerBasePureVirtualClass
    {
        public:

        /// @brief default virtual destructor
        virtual ~ComponentContainerBasePureVirtualClass() = default;

        /// @brief Remove a Component from an Entity
        ///
        /// Does nothing if the Entity doesn't have that Component
        /// @param cv_entity Entity in question
        virtual void remove(const Entity cv_entity) = 0;
    };

    /// @brief Templated Container Storage class for the Component Manager
    /// @tparam T type of Component to store with this class
    template<typename T>
    class ComponentContainer: public ComponentContainerBasePureVirtualClass
    {
        public:
        /// @brief insert a new Entity to this storage
        /// @param cv_entity Entity in question
        /// @param component Component (reference) to add
        inline void insert(const Entity cv_entity, T& component)
        {
            mContainer.insert[cv_entity] = component;
        }

        /// @brief Remove a Component from an Entity
        ///
        /// Does nothing if the Entity doesn't have that Component
        ///
        /// Is an Override of ComponentContainerBasePureVirtualClass::remove(const Entity)
        /// @param cv_entity Entity in question
        inline void remove(const Entity cv_entity) override 
        {
            mContainer.erase(cv_entity);
        }

        /// @brief get the Component of specified Entity
        /// @param cv_entity Entity in question
        /// @return reference to Component
        /// @warning This WILL crash if the Entity doesn't have that Component!!
        inline T& getData(const Entity cv_entity)
        {
            return mContainer[cv_entity];
        }

        private:
        /// @brief map of Entity to Component
        std::unordered_map<Entity, T> mContainer;
    };
}
#endif //LABYRINTH_PENDING_ENGINE_ECS_COMPONENTCONTAINER_HPP