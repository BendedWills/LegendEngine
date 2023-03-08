#ifdef VULKAN_API

#include <LegendEngine/Graphics/Vulkan/Surface.hpp>
#include <LegendEngine/Graphics/Vulkan/Instance.hpp>

#include <Tether/Native.hpp>

#ifdef __linux__
#include <vulkan/vulkan_xlib.h>
#elif _WIN32
#include <Windows.h>
#include <vulkan/vulkan_win32.h>
#endif


using namespace LegendEngine::Vulkan;
using namespace Tether;

bool Surface::Init(Instance* pInstance, Tether::Window* window)
{
    if (initialized)
        return false;
    
    this->pInstance = pInstance;

#ifdef __linux__
    VkXlibSurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    createInfo.dpy = Application::Get().GetStorage()->display;
    createInfo.window = window->GetStorage()->window;
    
    if (vkCreateXlibSurfaceKHR(pInstance->Get(), &createInfo, nullptr,
        &m_Surface) != VK_SUCCESS)
        return false;
#elif _WIN32
    Tether::Storage::VarStorage* varStorage = 
        ((Tether::Storage::VarStorage*)window->GetStorage());

    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = varStorage->window;
    createInfo.hinstance = varStorage->hinst;
    
    if (vkCreateWin32SurfaceKHR(pInstance->Get(), &createInfo, nullptr,
        &m_Surface) != VK_SUCCESS)
        return false;
#endif
    
    initialized = true;
    return true;
}

VkSurfaceKHR Surface::Get()
{
    return m_Surface;
}

void Surface::OnDispose()
{
    vkDestroySurfaceKHR(pInstance->Get(), m_Surface, nullptr);
}

#endif // VULKAN_API