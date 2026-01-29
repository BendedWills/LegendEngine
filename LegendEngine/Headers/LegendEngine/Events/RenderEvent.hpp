#pragma once

#include <LegendEngine/Events/Event.hpp>

namespace LegendEngine::Events
{
    class RenderEvent : public Event
    {
    public:
        explicit RenderEvent(float deltaTime);

        float GetDeltaTime() const;
    private:
        float m_DeltaTime;
    };
}