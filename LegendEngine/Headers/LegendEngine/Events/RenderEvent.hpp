#pragma once

#include <LegendEngine/Events/Event.hpp>

namespace le
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