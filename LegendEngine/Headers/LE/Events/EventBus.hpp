#pragma once

#include <typeindex>
#include <unordered_map>
#include <vector>

#include <LE/Events/EventBusSubscriber.hpp>

namespace le
{
    class Event;
    class EventBus final
    {
        friend class EventBusSubscriber;
    public:
        template<typename T>
            requires std::is_base_of_v<Event, T>
        void DispatchEvent(const T& event)
        {
            const std::vector<EventBusSubscriber*>& subscribers
                = m_Subscribers[typeid(T)];

            for (EventBusSubscriber* subscriber : subscribers)
            {
                subscriber->DispatchEvent(event);
                if (event.IsCanceled())
                    return;
            }
        }
    protected:
        void Subscribe(std::type_index eventID, EventBusSubscriber& subscriber);
        void Unsubscribe(std::type_index eventID, EventBusSubscriber& subscriber);
        void UnsubscribeAll(EventBusSubscriber& subscriber);
    private:
        std::unordered_map<std::type_index,
            std::vector<EventBusSubscriber*>> m_Subscribers{};
    };
}
