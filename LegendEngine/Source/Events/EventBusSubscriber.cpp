#include <LegendEngine/Events/EventBus.hpp>
#include <LegendEngine/Events/EventBusSubscriber.hpp>

namespace LegendEngine::Events
{
    EventBusSubscriber::EventBusSubscriber(EventBus& eventBus)
        :
        m_EventBus(eventBus)
    {}

    EventBusSubscriber::~EventBusSubscriber()
    {
        m_EventBus.UnsubscribeAll(*this);
    }
}