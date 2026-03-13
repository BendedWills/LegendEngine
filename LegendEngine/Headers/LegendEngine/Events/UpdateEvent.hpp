#pragma once

#include <LegendEngine/Events/Event.hpp>

namespace le
{
    class UpdateEvent : public Event
    {
    public:
        explicit UpdateEvent(float deltaTime);

        float GetDeltaTime() const;
    private:
        float m_DeltaTime;
    };
}