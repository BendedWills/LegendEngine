#include <ranges>
#include <LegendEngine/Events/EventBus.hpp>

namespace le
{
    void EventBus::UnsubscribeAll(EventBusSubscriber& subscriber)
    {
        for (auto& eventTypeVec: m_Subscribers | std::views::values)
            std::erase(eventTypeVec, &subscriber);
    }
}