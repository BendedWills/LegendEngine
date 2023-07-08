#include <LegendEngine/Application.hpp>
#include <LegendEngine/GraphicsContext.hpp>
#include <LegendEngine/Graphics/IRenderer.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Objects/Camera.hpp>
#include <LegendEngine/Objects/Scripts/Script.hpp>

#include <iostream>
#include <chrono>
#include <sstream>
#include <algorithm>

using namespace LegendEngine;

#define LEGENDENGINE_ASSERT_RENDERER_NULL() \
	if (!pRenderer) \
		return;

Application::EventHandler::EventHandler(Application* pApplication)
{
	this->pApplication = pApplication;
}

void Application::EventHandler::OnWindowResize(
	Tether::Events::WindowResizeEvent event
)
{
	this->pApplication->RecieveResize(event.GetNewWidth(), event.GetNewHeight());
}

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

bool Application::Init(
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
	this->applicationName = applicationName;
	this->logging = logging;
	this->debug = debug;
	
	Log("Initializing window", LogType::DEBUG);
	if (!InitWindow(applicationName))
	{
		Log("Failed to initialize window!", LogType::ERROR);
		return false;
	}

	initialized = true;

	*((Tether::Window**)(&pWindow)) = &window;

	if (!VulkanContext::InitAPI(api, debug))
		return false;

	switch (api)
	{
	#ifdef VULKAN_API
		case RenderingAPI::VULKAN: 
		{
			if (!InitVulkan())
				return false;
		}
		break;
	#endif // VULKAN_API
	}

	// Okay, yes, this is kind of stupid, but I really didn't want to have to call
	// GetRenderer all the time when I need the renderer. Even if I were to store the
	// renderer from GetRenderer, it's still messy and it is much easier to do:
	// application.renderer->DoAThing
	*((IRenderer**)&renderer) = pRenderer.get();

	InitScene(defaultScene);

	if (!OnPreInit())
		return false;

	// Show the window AFTER pre-initialization
	window.SetVisible(true);
	window.AddEventHandler(&eventHandler, Utils::Events::EventType::WINDOW_RESIZE);

	if (!OnInit())
		return false;

	Log("Initialized application", LogType::INFO);

	return true;
}

bool Application::Run()
{
	Stopwatch deltaTimer;
	while (!IsCloseRequested())
	{
		float delta = deltaTimer.GetElapsedMillis() / 1000.0f;
		deltaTimer.Set();

		RenderFrame(delta);
	}

	return true;
}

IRenderer* Application::GetRenderer()
{
	LEGENDENGINE_ASSERT_INITIALIZED_RET(NULL);
	return pRenderer.get();
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

Tether::Window* Application::GetWindow()
{
	LEGENDENGINE_ASSERT_INITIALIZED_RET(NULL);
	return &window;
}

bool Application::IsCloseRequested()
{
	LEGENDENGINE_ASSERT_INITIALIZED_RET(false);
	return window.IsCloseRequested();
}

void Application::UpdateWindow()
{
	LEGENDENGINE_ASSERT_INITIALIZED();

	window.PollEvents();
}

void Application::Update(float delta, bool updateWindow)
{
	LEGENDENGINE_ASSERT_INITIALIZED();

	if (updateWindow)
		UpdateWindow();
	
	OnUpdate(delta);

	for (uint64_t i = 0; i < updateScripts.size(); i++)
		updateScripts[i]->OnUpdate(delta);
}

void Application::Render(float delta)
{
	LEGENDENGINE_ASSERT_INITIALIZED();
	
	if (!pRenderer)
		return;

	for (uint64_t i = 0; i < renderUpdateScripts.size(); i++)
		renderUpdateScripts[i]->OnRender();
	
	pRenderer->RenderFrame();

	OnRendered(delta);
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

		std::tm* orgtimePtr;
	#if defined(__linux__)
		orgtimePtr = gmtime(&currentTime);
	#elif defined(_WIN32)
		std::tm organizedTime;
		gmtime_s(&organizedTime, &currentTime);

		orgtimePtr = &organizedTime;
	#endif

		time += std::to_string(orgtimePtr->tm_hour > 12 ? 
			orgtimePtr->tm_hour - 12 : orgtimePtr->tm_hour);
		time += ":";
		time += std::to_string(orgtimePtr->tm_min);
		time += ":";
		time += std::to_string(orgtimePtr->tm_sec);
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

Ref<VertexBuffer> Application::CreateVertexBuffer()
{
	LEGENDENGINE_ASSERT_INITIALIZED_RET(nullptr);

	Ref<VertexBuffer> vbuffer = RefTools::Create<VertexBuffer>();
	vbuffer->pApplication = this;

	if (!pRenderer->CreateVertexBufferNative(vbuffer.get()))
	{
		Log("Failed to create vertex buffer!", LogType::ERROR);
		return nullptr;
	}

	return vbuffer;
}

bool Application::InitScene(Scene& scene)
{
	return InitScene(&scene);
}

bool Application::InitScene(Scene* pScene)
{
	LEGENDENGINE_ASSERT_INITIALIZED_RET(false);

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
	str << "Scene (" << (uint64_t)pScene << ") initialized to Application";
	str << " (" << (uint64_t)this << ")";

	Log(str.str(), LogType::DEBUG);

	return true;
}

void Application::RenderFrame(float delta)
{
	LEGENDENGINE_ASSERT_INITIALIZED();

	Update(delta);
	Render(delta);
}

void Application::SetActiveCamera(Objects::Camera* pCamera)
{
	if (pCamera)
	{
		float aspect = (float)window.GetWidth() / (float)window.GetHeight();
		pCamera->SetAspectRatio(aspect);
	}

	pActiveCamera = pCamera;
}

Objects::Camera* Application::GetActiveCamera()
{
	return pActiveCamera;
}

Scene* Application::GetDefaultScene()
{
	LEGENDENGINE_ASSERT_INITIALIZED_RET(nullptr);

	return &defaultScene;
}

void Application::SetActiveScene(Scene& scene)
{
	SetActiveScene(&scene);
}

void Application::SetActiveScene(Scene* pScene)
{
	LEGENDENGINE_ASSERT_INITIALIZED();

	if (pScene)
		if (pScene->pApplication != this)
			return;

	pRenderer->OnSceneChange(pScene, nullptr);
	this->activeScene = pScene;
}

void Application::RemoveActiveScene()
{
	LEGENDENGINE_ASSERT_INITIALIZED();
	SetActiveScene(nullptr);
}

Scene* Application::GetActiveScene()
{
	LEGENDENGINE_ASSERT_INITIALIZED_RET(nullptr);
	return activeScene;
}

#ifdef VULKAN_API
bool Application::InitVulkan()
{
	LEGENDENGINE_ASSERT_INITIALIZED_RET(false);

	callback.pApplication = this;

	pRenderer = RefTools::Create<Vulkan::VulkanRenderer>();
	if (!pRenderer->Init(this))
	{
		Log("Failed to initialize Vulkan renderer!", LogType::ERROR);
		return false;
	}

	VulkanContext::GetVulkanInstance()->AddDebugMessenger(&callback);

	vulkanInitialized = true;
	return true;
}
#endif // VULKAN_API

bool Application::InitWindow(const std::string& title)
{
	window.Hint(Tether::HintType::VISIBLE, false);
	window.Hint(Tether::HintType::X, 120);
	window.Hint(Tether::HintType::Y, 120);
	if (!window.Init(1280, 720, title.c_str()))
		return false;
	
	return true;
}

void Application::DisposeGraphics()
{
	pRenderer->Dispose();

#ifdef VULKAN_API
	if (vulkanInitialized)
		VulkanContext::GetVulkanInstance()->RemoveDebugMessenger(&callback);
#endif // VULKAN_API
}

void Application::OnDispose()
{
	Log("Disposing application", LogType::INFO);

	window.SetVisible(false);

	OnStop();
	RemoveActiveScene();

	window.Dispose();

	// Dispose everything
	{
		// Objects are removed as they are disposed, so an original copy is required.
		std::vector<Objects::Object*> oldObjects(objects);
		for (uint64_t i = 0; i < oldObjects.size(); i++)
			oldObjects[i]->Dispose();

		// And same for resources
		std::vector<Resources::IResource*> oldResources(resources);
		for (uint64_t i = 0; i < oldResources.size(); i++)
			oldResources[i]->Dispose();

		// You get the idea
		std::vector<VertexBuffer*> oldVertexBuffers(vertexBuffers);
		for (uint64_t i = 0; i < oldVertexBuffers.size(); i++)
			oldVertexBuffers[i]->Dispose();
	}
	
	DisposeGraphics();

	defaultScene.ClearObjects();
	objects.clear();
	updateScripts.clear();
	renderUpdateScripts.clear();

	window.RemoveEventHandler(eventHandler);

	OnDisposed();
}

void Application::RecieveResize(uint64_t width, uint64_t height)
{
	pRenderer->OnWindowResize(width, height);

	float aspect = (float)width / (float)height;
	if (pActiveCamera)
		pActiveCamera->SetAspectRatio(aspect);

	OnResize(width, height);
}

void Application::_OnObjectDispose(Objects::Object* pObject)
{
	LEGENDENGINE_ASSERT_INITIALIZED();

	for (uint64_t i = 0; i < objects.size(); i++)
		if (objects[i] == pObject)
		{
			objects.erase(objects.begin() + i);
			break;
		}
}

void Application::SetScriptRecieveUpdates(
	bool enabled, Objects::Scripts::Script* pScript
)
{
	if (enabled)
	{
		for (uint64_t i = 0; i < updateScripts.size(); i++)
			if (updateScripts[i] == pScript)
				return;
		
		updateScripts.push_back(pScript);
		return;
	}
	
	for (uint64_t i = 0; i < updateScripts.size(); i++)
		if (updateScripts[i] == pScript)
			updateScripts.erase(updateScripts.begin() + i);
}

void Application::SetScriptRecieveRenderUpdates(
	bool enabled, Objects::Scripts::Script* pScript
)
{
	if (enabled)
	{
		for (uint64_t i = 0; i < renderUpdateScripts.size(); i++)
			if (renderUpdateScripts[i] == pScript)
				return;

		renderUpdateScripts.push_back(pScript);

		return;
	}

	for (uint64_t i = 0; i < renderUpdateScripts.size(); i++)
		if (renderUpdateScripts[i] == pScript)
			renderUpdateScripts.erase(renderUpdateScripts.begin() + i);
}
