#pragma once

namespace LegendEngine::Events
{
    class Event
    {
    public:
        explicit Event(bool cancelable = true);

        void SetCanceled(bool isCancelled);

        bool IsCanceled() const;
        bool IsCancelable() const;
    private:
        bool m_Cancelable;
        bool m_IsCancelled = false;
    };
}