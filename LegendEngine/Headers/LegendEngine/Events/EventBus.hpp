#pragma once

#include <LegendEngine/Events/Event.hpp>
#include <LegendEngine/Events/EventBusSubscriber.hpp>

#include <unordered_map>
#include <vector>
#include <typeindex>
#include <algorithm>

namespace LegendEngine::Events
{
    class EventBus final
    {
    public:
        // Adds a subscriber and adds the handler function to said subscriber
        template<typename T>
            requires std::is_base_of_v<Event, T>
        void Subscribe(EventBusSubscriber& subscriber, EventFunc<T> func)
        {
            subscriber.AddEventHandler(func);
            Subscribe<T>(subscriber);
        }

        // Adds a subscriber WITHOUT subscribing to the event bus, meaning it
        // won't receive events.
        template<typename T>
            requires std::is_base_of_v<Event, T>
        void Subscribe(EventBusSubscriber& subscriber)
        {
            std::vector<EventBusSubscriber*>& subscribers =
                m_Subscribers[typeid(T)];
            if (std::ranges::find(subscribers, &subscriber)
                != subscribers.end())
                return;

            m_Subscribers[typeid(T)].push_back(&subscriber);
        }

        template<typename T>
            requires std::is_base_of_v<Event, T>
        void Unsubscribe(EventBusSubscriber& subscriber)
        {
            std::vector<EventBusSubscriber*>& subscribers =
                m_Subscribers[typeid(T)];

            std::erase(subscribers, &subscriber);
        }

        void UnsubscribeAll(EventBusSubscriber& subscriber);

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
    private:
        std::unordered_map<std::type_index,
            std::vector<EventBusSubscriber*>> m_Subscribers;
    };
}
