#pragma once

#include <LegendEngine/Events/EventBus.hpp>

namespace LegendEngine
{
    namespace Objects
    {
        class Object;
    }

    class Script
    {
        friend class ScriptHolder;
        friend class Application;
    public:
        explicit Script(Objects::Object& object);
        virtual ~Script();

        virtual void OnUpdate(float deltaTime) {}
        virtual void OnRender(float deltaTime) {}

        Objects::Object& GetObject() const;
    protected:
        void ListenForUpdates();
        void ListenForRender();

        Objects::Object& m_Object;
    private:
        Events::EventBus& m_EventBus;
        Events::EventBusSubscriber m_EventSubscriber;

        bool m_IsUpdateListener = false;
        bool m_IsRenderListener = false;
    };
}