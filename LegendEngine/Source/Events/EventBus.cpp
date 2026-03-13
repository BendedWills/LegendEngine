#include <ranges>
#include <LegendEngine/Events/EventBus.hpp>

namespace le
{
    void EventBus::Subscribe(const std::type_index eventID, EventBusSubscriber& subscriber)
    {
        std::vector<EventBusSubscriber*>& subscribers =
                m_Subscribers[eventID];
        if (std::ranges::find(subscribers, &subscriber)
            != subscribers.end())
            return;

        m_Subscribers[eventID].push_back(&subscriber);
    }

    void EventBus::Unsubscribe(const std::type_index eventID, EventBusSubscriber& subscriber)
    {
        std::vector<EventBusSubscriber*>& subscribers =
            m_Subscribers[eventID];

        std::erase(subscribers, &subscriber);
    }

    void EventBus::UnsubscribeAll(EventBusSubscriber& subscriber)
    {
        for (auto& eventTypeVec: m_Subscribers | std::views::values)
            std::erase(eventTypeVec, &subscriber);
    }
}