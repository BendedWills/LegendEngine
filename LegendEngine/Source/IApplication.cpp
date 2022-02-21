#include <LegendEngine/IApplication.hpp>
#include <LegendEngine/Graphics/IRenderer.hpp>

#include <iostream>
#include <chrono>
#include <sstream>

using namespace LegendEngine;

void IApplication::DebugCallback::OnDebugLog(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
)
{
    std::stringstream ss;
    ss << "Vulkan Validation Layer: " << pCallbackData->pMessage;
    
    if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        pApplication->Log(ss.str(), LogType::ERROR);
    else
        pApplication->Log(ss.str(), LogType::DEBUG);
}

bool IApplication::Init(
    const std::string& applicationName,
    bool logging,
    bool debug,
    RenderingAPI api
)
{
    if (initialized)
    {
        Log("Tried to initialize twice, continuing...", LogType::DEBUG);
        return false;
    }
    
    this->pRenderer = nullptr;
    this->initializedApi = false;
    this->initializedVulkan = false;
    this->applicationName = applicationName;
    this->logging = logging;
    this->debug = debug;

    if (!OnPreInit())
        return false;
    
    Log("Initializing window", LogType::DEBUG);
    if (!InitWindow(applicationName))
    {
        Log("Failed to initialize window!", LogType::ERROR);
        return false;
    }

    initialized = true;

    initializedApi = true;
    switch (api)
    {
        case RenderingAPI::VULKAN: 
        {
            if (!InitVulkan(debug))
                return false;
        }
        break;

        case RenderingAPI::AUTO_SELECT:
        {
            if (!InitVulkan(debug))
                return false;
        }
        break;

        default:
        {
            initializedApi = false;
        }
        break;
    }

    if (!OnInit())
        return false;
    
    Log("Initialized application", LogType::INFO);

    window.SetVisible(true);

    return true;
}

bool IApplication::SetRenderer(IRenderer* pRenderer)
{
    LEGENDENGINE_ASSERT_INITIALIZED_RET(false);
    
    if (!pRenderer)
    {
        this->pRenderer = nullptr;
        return true;
    }

    if (!pRenderer->IsInitialized() || pRenderer->GetApplication() != this)
    {
        Log(
            "Tried to set renderer but the renderer was either unitialized or"
            " wasn't created for this application", 
            LogType::ERROR
        );

        return false;
    }
    
    this->pRenderer = pRenderer;
    return true;
}

IRenderer* IApplication::GetRenderer()
{
    LEGENDENGINE_ASSERT_INITIALIZED_RET(NULL);
    return pRenderer;
}

IApplication* IApplication::Get()
{
    return this;
}

std::string IApplication::GetName()
{
    LEGENDENGINE_ASSERT_INITIALIZED_RET("");
    return applicationName;
}

Tether::IWindow* IApplication::GetWindow()
{
    LEGENDENGINE_ASSERT_INITIALIZED_RET(NULL);
    return &window;
}

bool IApplication::IsCloseRequested()
{
    LEGENDENGINE_ASSERT_INITIALIZED_RET(false);
    return window.IsCloseRequested();
}

bool IApplication::IsApiInitialized()
{
    LEGENDENGINE_ASSERT_INITIALIZED_RET(false);
    return initializedApi;
}

void IApplication::SetFullscreen(bool fullscreen, int monitor)
{
    LEGENDENGINE_ASSERT_INITIALIZED();
    window.SetFullscreen(fullscreen, monitor);
}

bool IApplication::IsVulkanInitialized()
{
    LEGENDENGINE_ASSERT_INITIALIZED_RET(false);
    return initializedVulkan;
}

Vulkan::Instance* IApplication::GetVulkanInstance()
{
    return &instance;
}

void IApplication::UpdateWindow()
{
    LEGENDENGINE_ASSERT_INITIALIZED();

    window.PollEvents();
}

void IApplication::Update(bool updateWindow)
{
    LEGENDENGINE_ASSERT_INITIALIZED();

    if (updateWindow)
        UpdateWindow();
}

void IApplication::Render()
{
    LEGENDENGINE_ASSERT_INITIALIZED();
    
    if (!pRenderer)
        return;
    
    pRenderer->RenderFrame();
}

void IApplication::Log(const std::string& message, LogType type)
{
    // If debug isn't enabled, disable debug logs.
    if ((type == LogType::DEBUG && !debug) || !logging)
		return;
	
	const std::string reset = "\e[0m";

	std::string severity = "";
	std::string color = "";
	switch (type)
	{
		case LogType::INFO:
		{
			severity = "INFO";
			color = "\e[38;2;0;120;220m"; // Blue
		}
		break;

		case LogType::WARN:
		{
			severity = "WARN";
			color = "\e[38;2;255;213;0m"; // Gold
		}
		break;

		case LogType::DEBUG:
		{
			severity = "DEBUG";
			color = "\e[38;2;0;150;60m"; // Green
		}
		break;

		case LogType::ERROR:
		{
			severity = "ERROR";
			color = "\e[38;2;255;0;0m"; // Red
		}
		break;
	}

    // Time format is HH:MM:SS
	std::string time = "";
    {
        auto now = std::chrono::system_clock::now();
        std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
        std::tm* organizedTime = std::gmtime(&currentTime);
        time += std::to_string(organizedTime->tm_hour > 12 ? 
            organizedTime->tm_hour - 12 : organizedTime->tm_hour);
        time += ":";
        time += std::to_string(organizedTime->tm_min);
        time += ":";
        time += std::to_string(organizedTime->tm_sec);
    }

	std::cout 
        // Time
        << "\e[38;2;118;118;118m[\e[38;2;148;148;148m" 
        << time 
		<< "\e[38;2;118;118;118m] " 
        // IApplication name
        << "\e[38;2;118;118;118m[\e[38;2;148;148;148m"
        << applicationName
        << "\e[38;2;118;118;118m] " 
        << reset 
        << ": " << color 
        // Severity
        << "[" << severity << "] => " 
        << message << reset 
    << std::endl;
}

bool IApplication::InitVulkan(bool enableValidationLayers)
{
    LEGENDENGINE_ASSERT_INITIALIZED_RET(false);

    if (initializedVulkan)
        return false;
    
    callback.pApplication = this;
    
    instance.AddDebugMessenger(&callback);
    if (!instance.Init(GetName().c_str(), "LegendEngine", 
        enableValidationLayers))
        return false;
    
    initializedVulkan = true;
    return true;
}

bool IApplication::InitWindow(const std::string& title)
{
    window.Hint(Tether::HintType::VISIBLE, false);
    if (!window.Init(1280, 720, title.c_str()))
        return false;
    
    return true;
}

void IApplication::DisposeGraphics()
{
    instance.Dispose();
}

void IApplication::OnDispose()
{
    Log("Disposing application", LogType::INFO);

    OnStop();

    window.Dispose();
    
    DisposeGraphics();

    OnDisposed();
}