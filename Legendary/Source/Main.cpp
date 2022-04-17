#include <iostream>
#include <chrono>
#include <thread>

#include <LegendEngine/LegendEngine.hpp>

using namespace LegendEngine;
using namespace LegendEngine::Objects;
using namespace std::literals::chrono_literals;

class TestScript : public Scripts::Script
{
public:
	TestScript() {}

	void OnInit()
	{
		SetRecieveUpdates(true);

		// Add the mesh component
		{
			float r = (float)rand() / RAND_MAX;
			float g = (float)rand() / RAND_MAX;
			float b = (float)rand() / RAND_MAX;
			
			VertexTypes::Vertex2 testVertices[] =
			{
				{   -0.05f, -0.05f, r, g, b, },
				{   -0.05f,  0.05f, r, g, b, },
				{    0.05f, -0.05f, r, g, b, },

				{   -0.05f,  0.05f, r, g, b, },
				{    0.05f,  0.05f, r, g, b, },
				{    0.05f, -0.05f, r, g, b, }
			};

			pObject->AddComponent<Components::MeshComponent>()->Init(
				testVertices, 6
			);
		}

		float x = (float)rand() / RAND_MAX * 2 - 1;
		float y = (float)rand() / RAND_MAX * 2 - 1;
		x *= 1.1f;
		y *= 1.1f;

		pObject->SetPosition(Vector3f(x, y, 0));
	}

	void OnUpdate(float delta)
	{
		pObject->AddRotation(Vector3f(0, 0, 100.0f * delta));
	}
};

class CameraScript : public Scripts::Script, Input::InputListener
{
public:
	CameraScript() {}

	void OnInit()
	{
		SetRecieveUpdates(true);
		pApplication->pWindow->AddInputListener(this, Input::InputType::KEY);
		pApplication->pWindow->AddInputListener(this, Input::InputType::RAW_MOUSE_MOVE);
	}

	void OnUpdate(float delta)
	{
		float x = (float)rand() / RAND_MAX * 2 - 1;
		float y = (float)rand() / RAND_MAX * 2 - 1;
		float z = (float)rand() / RAND_MAX * 2 - 1;

		if (keys.w)
			pObject->AddPosition(Vector3f(1, 0, 0) * delta);
		if (keys.a)
			pObject->AddPosition(Vector3f(0, 0, -1) * delta);
		if (keys.s)
			pObject->AddPosition(Vector3f(-1, 0, 0) * delta);
		if (keys.d)
			pObject->AddPosition(Vector3f(0, 0, 1) * delta);
		
		// Vertical movement
		if (keys.space)
			pObject->AddPosition(Vector3f(0, 1, 0) * delta);
		if (keys.shift)
			pObject->AddPosition(Vector3f(0, -1, 0) * delta);
	}

	void OnKey(Input::KeyInfo& info)
	{
		bool pressed = info.IsPressed();
		switch (info.GetKey())
		{
			case Utils::Keycodes::KEY_W: keys.w = pressed; break;
			case Utils::Keycodes::KEY_A: keys.a = pressed; break;
			case Utils::Keycodes::KEY_S: keys.s = pressed; break;
			case Utils::Keycodes::KEY_D: keys.d = pressed; break;

			case Utils::Keycodes::KEY_SPACE: keys.space = pressed; break;
			case Utils::Keycodes::KEY_LEFT_SHIFT: keys.shift = pressed; break;
		}
	}

	void OnRawMouseMove(Input::RawMouseMoveInfo& info)
	{

	}

	struct Keys
	{
		bool w = false;
		bool a = false;
		bool s = false;
		bool d = false;
		bool space = false;
		bool shift = false;
	};
	Keys keys;
};

class Triangle : public Application
{
public:
	bool OnInit()
	{
		pWindow->SetCursorMode(Utils::CursorMode::DISABLED);
		pWindow->SetRawInputEnabled(true);

		InitScene(testScene);

		camera = CreateObject<Camera>();
		camera->AddScript<CameraScript>();

		SetActiveCamera(camera.get());

		float allTime = 0.0f;
		float time = 1.6f;
		Stopwatch timer;
		for (uint64_t i = 0; i < 100; i++)
		{
			Ref<Object> object = CreateObject<Object>();

			object->AddScript<TestScript>();

			objects.push_back(object);
			testScene.AddObject(object.get());

			float elapsed = timer.GetElapsedMillis();

			allTime += elapsed;
			time += elapsed;
			time /= 2;

			timer.Set();
		}

		Log("Average time: " + std::to_string(time), LogType::INFO);
		Log("Average time 2: " + std::to_string(allTime / 100.0f), LogType::INFO);
		Log("Total time: " + std::to_string(allTime), LogType::INFO);

		SetActiveScene(testScene);
		
		return true;
	}

	void OnStop()
	{
		testScene.ClearObjects();
		objects.clear();
	}
private:
	Scene testScene;

	Ref<Camera> camera;
	std::vector<Ref<Object>> objects;
};

#if defined(_WIN32) && !defined(_DEBUG)
#include <Windows.h>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine,
	int nCmdShow)
#else
int main()
#endif
{
	Context::InitAPI(RenderingAPI::VULKAN, true);
	{
		Triangle triangle;
		if (!triangle.Init("Legendary", true, true))
			return EXIT_FAILURE;

		Stopwatch deltaTimer;
		while (!triangle.IsCloseRequested())
		{
			float delta = deltaTimer.GetElapsedMillis() / 1000.0f;
			deltaTimer.Set();

			triangle.RenderFrame(delta);
		}

		triangle.Dispose();
	}
	Context::Dispose();

	std::cout << "\nPress any key to continue..." << std::endl;
	std::cin.get();

	return EXIT_SUCCESS;
}
