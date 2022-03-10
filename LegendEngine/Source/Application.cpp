#include <LegendEngine/Application.hpp>
#include <LegendEngine/Graphics/IRenderer.hpp>

#include <iostream>
#include <chrono>
#include <sstream>

using namespace LegendEngine;

void Application::DebugCallback::OnDebugLog(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
)
{
    std::stringstream ss;
    ss << "Vulkan Validation Layer: " << pCallbackData->pMessage;

    if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        pApplication->Log(ss.str(), LogType::ERROR);
    else if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        pApplication->Log(ss.str(), LogType::WARN);
}

bool Application::Start(
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

    InitScene(defaultScene);

    if (!OnInit())
        return false;
    
    Log("Initialized application", LogType::INFO);

    // Show the window AFTER initialization
    window.SetVisible(true);

    return StartLoop();
}

bool Application::SetRenderer(IRenderer* pRenderer)
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

IRenderer* Application::GetRenderer()
{
    LEGENDENGINE_ASSERT_INITIALIZED_RET(NULL);
    return pRenderer;
}

Application* Application::Get()
{
    return this;
}

std::string Application::GetName()
{
    LEGENDENGINE_ASSERT_INITIALIZED_RET("");
    return applicationName;
}

Tether::IWindow* Application::GetWindow()
{
    LEGENDENGINE_ASSERT_INITIALIZED_RET(NULL);
    return &window;
}

bool Application::IsCloseRequested()
{
    LEGENDENGINE_ASSERT_INITIALIZED_RET(false);
    return window.IsCloseRequested();
}

bool Application::IsApiInitialized()
{
    LEGENDENGINE_ASSERT_INITIALIZED_RET(false);
    return initializedApi;
}

void Application::SetFullscreen(bool fullscreen, int monitor)
{
    LEGENDENGINE_ASSERT_INITIALIZED();
    //window.SetFullscreen(fullscreen, monitor);
}

bool Application::IsVulkanInitialized()
{
    LEGENDENGINE_ASSERT_INITIALIZED_RET(false);
    return initializedVulkan;
}

Vulkan::Instance* Application::GetVulkanInstance()
{
    return &instance;
}

void Application::UpdateWindow()
{
    LEGENDENGINE_ASSERT_INITIALIZED();

    window.PollEvents();
}

void Application::Update(bool updateWindow)
{
    LEGENDENGINE_ASSERT_INITIALIZED();

    

    if (updateWindow)
        UpdateWindow();
    
    OnUpdate();
}

void Application::Render()
{
    LEGENDENGINE_ASSERT_INITIALIZED();
    
    if (!pRenderer)
        return;
    
    pRenderer->RenderFrame();
}

void Application::Log(const std::string& message, LogType type)
{
    // If debug isn't enabled, disable debug logs.
    if ((type == LogType::DEBUG && !debug) || !logging)
		return;
	
	const std::string reset = "\x1b[0m";

	std::string severity = "";
	std::string color = "";
	switch (type)
	{
		case LogType::INFO:
		{
			severity = "INFO";
			color = "\x1b[38;2;0;120;220m"; // Blue
		}
		break;

		case LogType::WARN:
		{
			severity = "WARN";
			color = "\x1b[38;2;255;213;0m"; // Gold
		}
		break;

		case LogType::DEBUG:
		{
			severity = "DEBUG";
			color = "\x1b[38;2;0;150;60m"; // Green
		}
		break;

		case LogType::ERROR:
		{
			severity = "ERROR";
			color = "\x1b[38;2;255;0;0m"; // Red
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
        << "\x1b[38;2;118;118;118m[\x1b[38;2;148;148;148m" 
        << time 
		<< "\x1b[38;2;118;118;118m] " 
        // Application name
        << "\x1b[38;2;118;118;118m[\x1b[38;2;148;148;148m"
        << applicationName
        << "\x1b[38;2;118;118;118m] " 
        << reset 
        << ": " << color 
        // Severity
        << "[" << severity << "] => " 
        << message << reset 
    << std::endl;
}

bool Application::InitObject(Objects::Object& object)
{
    return InitObject(&object);
}

bool Application::InitObject(Objects::Object* pObject)
{
    if (!pObject)
    {
        Log("Initializing object: Object is nullptr. Returning.",
            LogType::WARN);
        return false;
    }

    if (pObject->pApplication != nullptr)
    {
        std::stringstream str;
        str << "Object (" << (uint64_t)pObject << ") has already been initialized";
        str << " (initialized with pApplication = "
            << (uint64_t)pObject->pApplication;
        str << ")";

        Log(str.str(), LogType::WARN);

        return false;
    }

    pObject->pApplication = this;

    std::stringstream str;
    str << "Object (" << (uint64_t)pObject << ") initialized to Application3D";
    str << " (" << (uint64_t)this << ")";

    Log(str.str(), LogType::DEBUG);

    return true;
}

bool Application::InitScene(Scene& scene)
{
    return InitScene(&scene);
}

bool Application::InitScene(Scene* pScene)
{
    if (!pScene)
    {
        Log("Initializing scene: Scene is nullptr. Returning.",
            LogType::WARN);
        return false;
    }

    if (pScene->pApplication != nullptr)
    {
        std::stringstream str;
        str << "Scene (" << (uint64_t)pScene << ") has already been initialized";
        str << " (initialized with pApplication = "
            << (uint64_t)pScene->pApplication;
        str << ")";

        Log(str.str(), LogType::WARN);

        return false;
    }

    pScene->pApplication = this;

    std::stringstream str;
    str << "Scene (" << (uint64_t)pScene << ") initialized to Application3D";
    str << " (" << (uint64_t)this << ")";

    Log(str.str(), LogType::DEBUG);

    return true;
}

Scene* Application::GetDefaultScene()
{
    return &defaultScene;
}

void Application::SetActiveScene(Scene& scene)
{
    SetActiveScene(&scene);
}

void Application::SetActiveScene(Scene* pScene)
{
    if (!pScene && pRenderer)
        pRenderer->OnSceneRemove(pScene);

    this->activeScene = pScene;

    if (pRenderer)
        pRenderer->OnSceneChange(pScene);
}

void Application::RemoveActiveScene()
{
    SetActiveScene(nullptr);
}

Scene* Application::GetActiveScene()
{
    return activeScene;
}

bool Application::InitVulkan(bool enableValidationLayers)
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

bool Application::InitWindow(const std::string& title)
{
    window.Hint(Tether::HintType::VISIBLE, false);
    window.Hint(Tether::HintType::X, 120);
    window.Hint(Tether::HintType::Y, 120);
    if (!window.Init(1280, 720, title.c_str()))
        return false;
    
    return true;
}

bool Application::StartLoop()
{
    while (!IsCloseRequested())
	{
		Update();
		Render();
	}

    Dispose();

    return true;
}

void Application::DisposeGraphics()
{
    instance.Dispose();
}

void Application::OnDispose()
{
    Log("Disposing application", LogType::INFO);

    OnStop();
    RemoveActiveScene();

    window.Dispose();
    
    DisposeGraphics();

    OnDisposed();
}

void Application::OnSceneObjectAdd(Scene* pScene,
    Objects::Object* pObject)
{
    if (activeScene != pScene)
        return;

    if (pScene == &defaultScene)
        pRenderer->OnDefaultObjectAdd(pScene, pObject);
    else
        pRenderer->OnSceneObjectAdd(pScene, pObject);
}

void Application::OnSceneObjectRemove(Scene* pScene,
    Objects::Object* pObject)
{
    if (activeScene != pScene)
        return;

    if (pScene == &defaultScene)
        pRenderer->OnDefaultObjectRemove(pScene, pObject);
    else
        pRenderer->OnSceneObjectRemove(pScene, pObject);
}