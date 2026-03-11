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
    {
        LGENG_DEBUG("Script created (this = {:#X}, &object = {:#X})",
            reinterpret_cast<size_t>(this), reinterpret_cast<size_t>(&object));
    }

    Script::~Script()
    {
        LGENG_DEBUG("Script destroyed (this = {:#X}, &object = {:#X})",
        reinterpret_cast<size_t>(this), reinterpret_cast<size_t>(&m_Object));
    }

    Objects::Object& Script::GetObject() const
    {
        return m_Object;
    }

    void Script::ListenForUpdates()
    {
        LGENG_DEBUG("Script {:#X} is listening for updates",
            reinterpret_cast<size_t>(this));

        m_IsUpdateListener = true;

        m_EventBus.Subscribe<Events::UpdateEvent>(m_EventSubscriber,
        [this](const Events::UpdateEvent& event)
        {
            OnUpdate(event.GetDeltaTime());
        });
    }

    void Script::ListenForRender()
    {
        LGENG_DEBUG("Script {:#X} is listening for renders",
            reinterpret_cast<size_t>(this));

        m_IsRenderListener = true;

        m_EventBus.Subscribe<Events::RenderEvent>(m_EventSubscriber,
        [this](const Events::RenderEvent& event)
        {
            OnUpdate(event.GetDeltaTime());
        });
    }
}
