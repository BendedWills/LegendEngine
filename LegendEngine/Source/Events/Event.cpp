#include <LegendEngine/Application.hpp>
#include <LegendEngine/Events/Event.hpp>

namespace LegendEngine::Events
{
    Event::Event(const bool cancelable)
        :
        m_Cancelable(cancelable)
    {}

    void Event::SetCanceled(const bool isCancelled)
    {
        if (!m_Cancelable && isCancelled)
        {
            Application::Get().GetLogger().Log(Logger::Level::WARN,
                "An event was set to be canceled, but it was not cancelable");
            return;
        }

        m_IsCancelled = isCancelled;
    }

    bool Event::IsCanceled() const
    {
        return m_IsCancelled;
    }

    bool Event::IsCancelable() const
    {
        return m_Cancelable;
    }
}