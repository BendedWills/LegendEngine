#include <LegendEngine/Script.hpp>
#include <LegendEngine/Application.hpp>

#include <LegendEngine/Events/UpdateEvent.hpp>
#include <LegendEngine/Events/RenderEvent.hpp>

namespace le
{
    Script::Script(Object& object)
        :
        m_Object(object),
        m_EventBus(Application::Get().GetEventBus()),
        m_EventSubscriber(m_EventBus)
    {
        LE_DEBUG("Script created (this = {:#x}, &object = {:#x})",
            reinterpret_cast<size_t>(this), reinterpret_cast<size_t>(&object));
    }

    Script::~Script()
    {
        LE_DEBUG("Script destroyed (this = {:#x}, &object = {:#x})",
        reinterpret_cast<size_t>(this), reinterpret_cast<size_t>(&m_Object));
    }

    Object& Script::GetObject() const
    {
        return m_Object;
    }

    void Script::ListenForUpdates()
    {
        LE_DEBUG("Script {:#x} is listening for updates",
            reinterpret_cast<size_t>(this));

        m_IsUpdateListener = true;

        m_EventSubscriber.AddEventHandler<UpdateEvent>(
        [this](const UpdateEvent& event)
        {
            OnUpdate(event.GetDeltaTime());
        });
    }

    void Script::ListenForRender()
    {
        LE_DEBUG("Script {:#x} is listening for renders",
            reinterpret_cast<size_t>(this));

        m_IsRenderListener = true;

        m_EventSubscriber.AddEventHandler<RenderEvent>(
        [this](const RenderEvent& event)
        {
            OnUpdate(event.GetDeltaTime());
        });
    }
}
