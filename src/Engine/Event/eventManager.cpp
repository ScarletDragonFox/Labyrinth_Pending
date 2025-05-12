#include "Labyrinth/Engine/Event/eventManager.hpp"
#include <iostream>



namespace lp
{
    EventListenerID EventManager::on(const EventTypes cv_eventtype, const EventListener& cr_listener)
    {
        if(cv_eventtype == EventTypes::Count || cv_eventtype == EventTypes::Invalid) return 0;
        std::uint32_t ret = static_cast<std::uint32_t>(cv_eventtype);
        //std::cout << "eventtype: " << ret << ", size = " << mListeners[static_cast<std::size_t>(cv_eventtype)].size() << "\n";
        ret = (ret << 16) + static_cast<std::uint16_t>(mListeners[static_cast<std::size_t>(cv_eventtype)].size());
        mListeners[static_cast<std::size_t>(cv_eventtype)].push_back(cr_listener);
        //TODO: log this!
        //std::cout << "New listener: id = " << ret << "\n";

        return ret;
    }

    void EventManager::unregister(const EventListenerID cv_listenerID)
    {
        const std::uint32_t tt = cv_listenerID;
        const std::uint16_t* arr2 = (const std::uint16_t*)&tt;
        //EventTypes type = static_cast<EventTypes>(arr2[1]);

        auto& vect = this->mListeners[arr2[1]];
        std::swap(vect.at(vect.size() - 1), vect.at(arr2[0]));
        vect.resize(vect.size() - 1);
    }

    void EventManager::emit(Event& cv_event)
    {
        EventTypes type = cv_event.getEventType();
        if(type == EventTypes::Count || type == EventTypes::Invalid)return;
        for(auto& func: this->mListeners[static_cast<std::size_t>(type)])
        {
            func(cv_event);
        }
    }

}
