#include <LegendEngine/Application.hpp>
#include <LegendEngine/IRenderer.hpp>

#include <iostream>
#include <chrono>

using namespace LegendEngine;

bool Application::Init(
    const std::string& applicationName,
    bool logging,
    bool debug
)
{
    if (initialized)
    {
        Log("Tried to initialize twice, continuing...", LogType::DEBUG);
        return false;
    }
    
    this->applicationName = applicationName;
    this->logging = logging;
    this->debug = debug;

    OnPreInit();
    
    Log("Initializing window", LogType::DEBUG);
    if (!InitWindow(applicationName))
    {
        Log("Failed to initialize window!", LogType::ERROR);
        return false;
    }

    initialized = true;

    OnInit();
    Log("Initialized application", LogType::INFO);

    window.SetVisible(true);

    return true;
}

bool Application::SetRenderer(IRenderer* pRenderer)
{
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

Application* Application::Get()
{
    return this;
}

std::string Application::GetName()
{
    return applicationName;
}

Tether::IWindow* Application::GetWindow()
{
    return &window;
}

bool Application::IsCloseRequested()
{
    return window.IsCloseRequested();
}

bool Application::ProcessFrame()
{
    Update();
    Render();

    return true;
}

void Application::Log(const std::string& message, LogType type)
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
        // Application name
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

bool Application::InitWindow(const std::string& title)
{
    window.Hint(Tether::HintType::VISIBLE, false);
    if (!window.Init(1280, 720, title.c_str()))
        return false;
    
    return true;
}

void Application::Update()
{
    window.PollEvents();
}

void Application::Render()
{
    
}

void Application::OnDispose()
{
    Log("Disposing application", LogType::INFO);
    OnStop();
}