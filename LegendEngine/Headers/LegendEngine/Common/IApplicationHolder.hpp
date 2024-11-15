/*
 * IApplicationHolder is an interface for classes that need access to their parent
 * Application.
 * 
 * It has an application variable and a GetApplication function.
 */

#ifndef _LEGENDENGINE_IAPPLICATIONHOLDER_HPP
#define _LEGENDENGINE_IAPPLICATIONHOLDER_HPP

namespace LegendEngine
{
    class Application;
    class IApplicationHolder
    {
        friend Application;
    public:
        Application* GetApplication();
    protected:
        Application* pApplication = nullptr;
    };
}

#endif //_LEGENDENGINE_IDISPOSABLE_HPP