/*
 * This header defines the surface. Surfaces in Vulkan are interfaces with
 * the window.
 */

#ifndef _LEGENDENGINE_VULKAN_SURFACE_HPP
#define _LEGENDENGINE_VULKAN_SURFACE_HPP
#ifdef VULKAN_API

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/IDisposable.hpp>
#include <Tether/Tether.hpp>

#include <vulkan/vulkan.h>

namespace LegendEngine::Vulkan
{
    class Instance;
    class Surface : public IDisposable
    {
    public:
        Surface() {}
        LEGENDENGINE_NO_COPY(Surface);

        /**
         * @brief Initializes the surface object.
         * 
         * @param pInstance A pointer to the Vulkan instance.
         * @param window The window object.
         * 
         * @returns True if initialization succeeded. Otherwise, false.
         */
        bool Init(Instance* pInstance, Tether::IWindow* window);

        VkSurfaceKHR Get();
    protected:
        void OnDispose();
    private:
        Instance* pInstance;
        VkSurfaceKHR surface;
    };
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKAN_SURFACE_HPP
