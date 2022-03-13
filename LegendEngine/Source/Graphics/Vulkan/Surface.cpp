#ifdef VULKAN_API

#include <LegendEngine/Graphics/Vulkan/Surface.hpp>
#include <LegendEngine/Graphics/Vulkan/Instance.hpp>

#ifdef __linux__
#include <vulkan/vulkan_xlib.h>
#elif _WIN32
#include <Windows.h>
#include <vulkan/vulkan_win32.h>
#endif

#include <Tether/Common/Storage.hpp>

using namespace LegendEngine::Vulkan;

bool Surface::Init(Instance* pInstance, Tether::IWindow* window)
{
    if (initialized)
        return false;
    
    this->pInstance = pInstance;

#ifdef __linux__
    VkXlibSurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    createInfo.dpy = window->GetDisplay();
    createInfo.window = window->GetHandle();
    
    if (vkCreateXlibSurfaceKHR(pInstance->Get(), &createInfo, nullptr,
        &surface) != VK_SUCCESS)
        return false;
#elif _WIN32
    Tether::Storage::WindowsVarStorage* varStorage = 
        ((Tether::Storage::WindowsVarStorage*)window->GetStorage());

    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = varStorage->window;
    createInfo.hinstance = varStorage->hinst;
    
    if (vkCreateWin32SurfaceKHR(pInstance->Get(), &createInfo, nullptr,
        &surface) != VK_SUCCESS)
        return false;
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