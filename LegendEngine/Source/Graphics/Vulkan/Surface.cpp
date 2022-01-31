#ifdef VULKAN_API

#include <LegendEngine/Graphics/Vulkan/Surface.hpp>
#include <LegendEngine/Graphics/Vulkan/Instance.hpp>

#include <vulkan/vulkan_xlib.h>

using namespace LegendEngine::Vulkan;

bool Surface::Init(Instance* pInstance, Tether::IWindow* window)
{
    if (initialized)
        return false;
    
    this->pInstance = pInstance;

    // if (glfwCreateWindowSurface(
    //         pInstance->Get(),
    //         window,
    //         nullptr,
    //         &surface
    //     ) != VK_SUCCESS)
    //     return false;

#ifdef __linux__
    VkXlibSurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    createInfo.dpy = Tether::Application::GetDisplay();
    createInfo.window = window->GetHandle();
    
    if (vkCreateXlibSurfaceKHR(pInstance->Get(), &createInfo, nullptr,
        &surface) != VK_SUCCESS)
        return false;
#elif _WIN32
    #error "Surface creation for Windows is not yet supported!"
#endif
    
    initialized = true;
    return true;
}

VkSurfaceKHR Surface::Get()
{
    return surface;
}

void Surface::OnDispose()
{
    vkDestroySurfaceKHR(pInstance->Get(), surface, nullptr);
}

#endif // VULKAN_API