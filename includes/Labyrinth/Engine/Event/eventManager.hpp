#ifndef LABYRINTH_PENDING_ENGINE_EVENTMANAGER_HPP
#define LABYRINTH_PENDING_ENGINE_EVENTMANAGER_HPP
#include "types.hpp"
#include "event.hpp"

#include <functional>
#include <array>
#include <vector>

namespace lp
{
    using EventListener = std::function<void(Event&)>;
    using EventListenerID = std::uint32_t;

    class EventManager
    {
        public:
        /// @brief register a new event listener
        /// @param cv_eventtype type of event to listen for
        /// @param cr_listener listener in question
        /// @return event listeners id - used for deleting/unegistering it. 0 is an invalid value, used for errors
        EventListenerID on(const EventTypes cv_eventtype, const EventListener& cr_listener);

        /// @brief unregister an event listener
        /// @param cv_listenerID event listeners id
        /// @note does nothing if the ID is invalid
        void unregister(const EventListenerID cv_listenerID);

        /// @brief emits (sends out) an event
        ///
        /// This will stop execution and handle all the event listeners first before proceeding
        /// @param cv_event event to send. Contains the type of event in question as well
        void emit(Event& cv_event);

        private:
        std::array<std::vector<EventListener>, static_cast<std::size_t>(EventTypes::Count)> mListeners;
    };
}

#endif //LABYRINTH_PENDING_ENGINE_EVENTMANAGER_HPP