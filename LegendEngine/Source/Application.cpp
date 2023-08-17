#include <LegendEngine/Application.hpp>
#include <LegendEngine/GraphicsContext.hpp>
#include <LegendEngine/Graphics/IRenderer.hpp>
#include <LegendEngine/Objects/Camera.hpp>
#include <LegendEngine/Objects/Scripts/Script.hpp>

#include <iostream>
#include <chrono>
#include <sstream>
#include <codecvt>
#include <locale>
#include <algorithm>

namespace LegendEngine
{
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

	Application::Application(
		const std::string& applicationName,
		bool logging,
		bool debug,
		RenderingAPI api
	)
		:
		eventHandler(this),
		applicationName(applicationName),
		logging(logging),
		debug(debug)
	{
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

		Log("Creating window", LogType::DEBUG);
		m_Window = Tether::Window::Create(1280, 720, 
			converter.from_bytes(applicationName), false);
		
		GraphicsContext::Create(api, debug);
		GraphicsContext& graphicsContext = GraphicsContext::Get();
		m_Renderer = graphicsContext.CreateRenderer(*this, *m_Window);

		InitScene(defaultScene);
	}

	Application::~Application()
	{
		Log("Disposing application", LogType::INFO);

		m_Window->SetVisible(false);

		RemoveActiveScene();

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

	IRenderer& Application::GetRenderer()
	{
		return *m_Renderer;
	}

	std::string Application::GetName()
	{
		return applicationName;
	}

	Tether::Window& Application::GetWindow()
	{
		return *m_Window;
	}

	bool Application::IsCloseRequested()
	{
		return m_Window->IsCloseRequested();
	}

	void Application::Update(float delta, bool updateWindow)
	{
		if (updateWindow)
			Tether::Application::Get().PollEvents();

		OnUpdate(delta);

		for (uint64_t i = 0; i < updateScripts.size(); i++)
			updateScripts[i]->OnUpdate(delta);
	}

	void Application::Render(float delta)
	{
		if (!m_Renderer)
			return;

		for (uint64_t i = 0; i < renderUpdateScripts.size(); i++)
			renderUpdateScripts[i]->OnRender();

		m_Renderer->RenderFrame();

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
		Ref<VertexBuffer> vbuffer = RefTools::Create<VertexBuffer>();
		vbuffer->pApplication = this;

		if (!m_Renderer->CreateVertexBufferNative(vbuffer.get()))
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
		Update(delta);
		Render(delta);
	}

	void Application::SetActiveCamera(Objects::Camera* pCamera)
	{
		if (pCamera)
		{
			float aspect = (float)m_Window->GetWidth() / (float)m_Window->GetHeight();
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
		return &defaultScene;
	}

	void Application::SetActiveScene(Scene& scene)
	{
		SetActiveScene(&scene);
	}

	void Application::SetActiveScene(Scene* pScene)
	{
		if (pScene)
			if (pScene->pApplication != this)
				return;

		m_Renderer->OnSceneChange(pScene, nullptr);
		this->activeScene = pScene;
	}

	void Application::RemoveActiveScene()
	{
		SetActiveScene(nullptr);
	}

	Scene* Application::GetActiveScene()
	{
		return activeScene;
	}

	void Application::DisposeGraphics()
	{
		m_Renderer->Dispose();
	}

	void Application::RecieveResize(uint64_t width, uint64_t height)
	{
		m_Renderer->OnWindowResize(width, height);

		float aspect = (float)width / (float)height;
		if (pActiveCamera)
			pActiveCamera->SetAspectRatio(aspect);

		OnResize(width, height);
	}

	void Application::FinishCreation()
	{
		m_Window->SetVisible(true);
		m_Window->AddEventHandler(eventHandler, Utils::Events::EventType::WINDOW_RESIZE);

		Log("Initialized application", LogType::INFO);
	}

	void Application::_OnObjectDispose(Objects::Object* pObject)
	{
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
}
