#include <iostream>

#include <Tether/Tether.hpp>

#include <thread>
#include <chrono>
#include <math.h>

using namespace std::literals::chrono_literals;
using namespace Tether;

class TestWindow : public Tether::Window
{
public:
	class EventHandler : public Events::EventHandler
	{
	public:
		EventHandler(TestWindow* pWindow)
			:
			pWindow(pWindow)
		{}

		void OnWindowClosing(Events::WindowClosingEvent event)
		{
			pWindow->SetVisible(false);
		}

		void OnWindowError(Events::WindowErrorEvent event)
		{
			std::cout << "window error: " << std::endl;
			std::cout << "\tERROR    = " << (int)event.GetCode() << std::endl;
			std::cout << "\tSEVERITY = " << (int)event.GetSeverity() 
				<< std::endl;
			std::cout << "\tFUNC_NAME = " << event.GetFunctionName() 
				<< std::endl;
		}
	private:
		TestWindow* pWindow = nullptr;
	};

	TestWindow()
		:
		handler(this)
	{
	}

	~TestWindow()
	{
		RemoveEventHandler(handler);
	}

	void InitializeComponent()
	{
		AddEventHandler(handler, Events::EventType::WINDOW_CLOSING);
		AddEventHandler(handler, Events::EventType::WINDOW_ERROR);
		SetBackgroundColor(Color(0.1f, 0.1f, 0.1f));

		panel.SetX(100);
		panel.SetY(100);
		panel.SetWidth(100);
		panel.SetHeight(100);
		panel.SetBackgroundColor(Color(255));
		AddControl(&panel);
	}
private:
	Controls::Panel panel;
	EventHandler handler;
};

int main()
{
	if (!Application::Init())
	{
		std::cout << "Failed to initialize window" << std::endl;
		return 1;
	}

	TestWindow window;
	window.Hint(HintType::API, TETHER_RENDERING_API_NONE);
	window.Hint(HintType::X, 120);
	window.Hint(HintType::Y, 120);
	if (!window.Init(1280, 720, "sup"))
	{
		std::cout << "Failed to initialize window" << std::endl;
		return 1;
	}

	while (!window.IsCloseRequested())
	{
		window.PollEvents();
		std::this_thread::sleep_for(1ms);
	}

	window.Dispose();
	
	Application::Dispose();
	return 0;
}