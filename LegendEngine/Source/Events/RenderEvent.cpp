#include <LegendEngine/Events/RenderEvent.hpp>

namespace LegendEngine::Events
{
    RenderEvent::RenderEvent(float deltaTime)
        :
        Event(false),
        m_DeltaTime(deltaTime)
    {}

    float RenderEvent::GetDeltaTime() const
    {
        return m_DeltaTime;
    }
}