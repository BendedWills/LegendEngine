#pragma once

#include <LegendEngine/Events/Event.hpp>

#include <unordered_map>
#include <typeindex>
#include <functional>

namespace LegendEngine::Events
{
    template<typename T>
    using EventFunc = std::function<void(const T&)>;

    class EventBus;
    class EventBusSubscriber final
    {
    public:
        explicit EventBusSubscriber(EventBus& eventBus);
        ~EventBusSubscriber();

        // Adds an event handler without subscribing to the event bus
        // Use EventBus::Subscribe(subscriber, func) to also subscribe
        // To the event bus
        template<typename T>
            requires std::is_base_of_v<Event, T>
        void AddEventHandler(const EventFunc<T> func)
        {
            if (m_Handlers.contains(typeid(T)))
                return;

            m_Handlers[typeid(T)] = [func](const Event& event)
            {
                func(static_cast<const T&>(event));
            };
        }

        template<typename T>
            requires std::is_base_of_v<Event, T>
        void RemoveEventHandler()
        {
            m_Handlers.erase(typeid(T));
        }

        template<typename T>
            requires std::is_base_of_v<Event, T>
        void DispatchEvent(T& event)
        {
            m_Handlers[typeid(T)](event);
        }
    private:
        EventBus& m_EventBus;

        std::unordered_map<std::type_index, EventFunc<Event>> m_Handlers;
    };
}
