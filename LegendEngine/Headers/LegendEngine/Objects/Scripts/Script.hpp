#ifndef _LEGENDENGINE_SCRIPT_HPP
#define _LEGENDENGINE_SCRIPT_HPP

#include <LegendEngine/Common/Defs.hpp>

namespace LegendEngine
{
    class Application;

    namespace Objects
    {
        class Object;
    }
}

namespace LegendEngine::Objects::Scripts
{
    class ScriptHolder;
    class Script
    {
        friend ScriptHolder;
        friend Application;
    public:
        Script()
        {}
        virtual ~Script()
        {
            OnDispose();
        }

        LEGENDENGINE_NO_COPY(Script);

        void SetRecieveUpdates(bool enabled);
        void SetRecieveRenderUpdates(bool enabled);
        
        Object* GetObject();
    protected:
        /**
         * @brief Called when the script is created in a script holder
         */
        virtual void OnInit() {}
        /**
         * @brief Called when the Application's Update function is called
         *  after OnUpdate. SetRecieveUpdates(true) must be executed for this function
         *  to be called.
         */
        virtual void OnUpdate(float delta) {}
        /**
         * @brief Called when the Application's Render function is called.
         *  SetRecieveRenderUpdates(true) must be executed for this function to be
         *  called.
         */
        virtual void OnRender() {}
        /**
         * @brief Called when the parent object is disposed.
         */
        virtual void OnDispose() {}

        Object* pObject = nullptr;

        bool updates = false;
        bool renderUpdates = false;
    };
}

#endif //_LEGENDENGINE_COMPONENTS_HPP