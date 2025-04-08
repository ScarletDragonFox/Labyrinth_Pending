#ifndef LABYRINTH_PENDING_ENGINE_EVENT_HPP
#define LABYRINTH_PENDING_ENGINE_EVENT_HPP
#include "types.hpp"
#include <any>

namespace lp
{
    /// @brief represents a singular event
    class Event
    {
        public:
        /// @brief default constructor deleted to avoid invalid events
        Event() = delete;

        /// @brief constructs an Event and stores its data
        /// @tparam T type of 'value', doesn't need to be specified
        /// @param cv_eventt type of the event
        /// @param value data this event will store
        template<typename T>
        Event(const EventTypes cv_eventt, T value)
            : mType(cv_eventt)
        { mData = value; }

        /// @brief constructs an Event without any data
        /// @param cv_eventt type of the event
        Event(const EventTypes cv_eventt)
            : mType(cv_eventt)
        {}

        /// @brief get this event's type
        /// @return EventTypes enum
        EventTypes getEventType() const { return mType; }

        /// @brief get the data as type T
        /// @tparam T the type to cast the data to from std::any
        /// @return the data this event stored (if any)
        /// @throws std::bad_any_cast - when the data is not of type T
        template<typename T>
        T getData()
        {
            return std::any_cast<T>(mData);
        }

        private:
        std::any mData = {}; //data storage for this event
        EventTypes mType = EventTypes::Invalid; //type of this event
    };
}

#endif //LABYRINTH_PENDING_ENGINE_EVENT_HPP