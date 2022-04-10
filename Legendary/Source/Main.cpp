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
				{   0.0f,   0.00f, r, g, b, },
				{   0.0f,   0.05f, r, g, b, },
				{   0.05f,  0.00f, r, g, b, },

				{   0.0f,   0.05f, r, g, b, },
				{   0.05f,  0.05f, r, g, b, },
				{   0.05f,  0.00f, r, g, b, }
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
		pObject->SetScale(Vector3f(2));
	}

	void OnUpdate(float delta)
	{
		pObject->AddRotation(Vector3f(0, 0, 0.1f * delta));
	}
};

class CameraScript : public Scripts::Script
{
public:
	CameraScript() {}

	void OnInit()
	{
		SetRecieveUpdates(true);
	}

	void OnUpdate(float delta)
	{
		float x = (float)rand() / RAND_MAX * 2 - 1;
		float y = (float)rand() / RAND_MAX * 2 - 1;
		float z = (float)rand() / RAND_MAX * 2 - 1;

		float rad = Math::Radians(rot);

		//pObject->AddRotation(Vector3f(0, 0, 0.1f * delta));
		pObject->SetPosition(Vector3f(cos(rad) * dist, 0, -sin(rad) * dist));

		rot += 0.05f * delta;
	}

	float rot = 0.0f;
	const float dist = 1.0f;
};

class Triangle : public Application
{
public:
	bool OnInit()
	{
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
			float delta = deltaTimer.GetElapsedMillis();
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
