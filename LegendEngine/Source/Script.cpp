#include <LegendEngine/Script.hpp>
#include <LegendEngine/Application.hpp>

#include <LegendEngine/Events/UpdateEvent.hpp>
#include <LegendEngine/Events/RenderEvent.hpp>

namespace LegendEngine
{
    Script::Script(Objects::Object& object)
        :
        m_Object(object),
        m_EventBus(Application::Get().GetEventBus()),
        m_EventSubscriber(m_EventBus)
    {}

    Script::~Script()
    {}

    Objects::Object& Script::GetObject() const
    {
        return m_Object;
    }

    void Script::ListenForUpdates()
    {
        m_IsUpdateListener = true;

        m_EventBus.Subscribe<Events::UpdateEvent>(m_EventSubscriber,
        [this](const Events::UpdateEvent& event)
        {
            OnUpdate(event.GetDeltaTime());
        });
    }

    void Script::ListenForRender()
    {
        m_IsRenderListener = true;

        m_EventBus.Subscribe<Events::RenderEvent>(m_EventSubscriber,
        [this](const Events::RenderEvent& event)
        {
            OnUpdate(event.GetDeltaTime());
        });
    }
}
