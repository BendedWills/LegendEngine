#pragma once

#include <LegendEngine/Events/EventBus.hpp>

namespace le
{
    class Object;

    class Script
    {
        friend class ScriptHolder;
        friend class Application;
    public:
        explicit Script(Object& object);
        virtual ~Script();

        virtual void OnUpdate(float deltaTime) {}
        virtual void OnRender(float deltaTime) {}

        Object& GetObject() const;
    protected:
        void ListenForUpdates();
        void ListenForRender();

        Object& m_Object;
    private:
        EventBus& m_EventBus;
        EventBusSubscriber m_EventSubscriber;

        bool m_IsUpdateListener = false;
        bool m_IsRenderListener = false;
    };
}