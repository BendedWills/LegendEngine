#include <LegendEngine/Events/UpdateEvent.hpp>

namespace LegendEngine::Events
{
    UpdateEvent::UpdateEvent(float deltaTime)
        :
        Event(false),
        m_DeltaTime(deltaTime)
    {}

    float UpdateEvent::GetDeltaTime() const
    {
        return m_DeltaTime;
    }
}