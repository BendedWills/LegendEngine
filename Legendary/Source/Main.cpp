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

		float x = (float)rand() / RAND_MAX * 2 - 1;
		float y = (float)rand() / RAND_MAX * 2 - 1;

		float r = (float)rand() / RAND_MAX;
		float g = (float)rand() / RAND_MAX;
		float b = (float)rand() / RAND_MAX;

		VertexTypes::Vertex2 testVertices[] =
		{
			{   0.0f,  -0.05f, r, g, b, },
			{   0.05f,  0.05f, r, g, b, },
			{  -0.05f,  0.05f, r, g, b, }
		};

		pObject->AddComponent<Components::MeshComponent>()->Init(
			testVertices, 3
		);
		pObject->SetPosition(Vector3f(x, y, 0));
	}

	void OnUpdate(float delta)
	{
		pObject->AddRotation(Vector3f(0, 0, 0.1f * delta));
	}
};

class Triangle : public Application
{
public:
	bool OnInit()
	{
		InitScene(testScene);

		for (uint64_t i = 0; i < 10; i++)
		{
			Ref<Object> object = CreateObject<Object>();

			object->AddScript<TestScript>();

			objects.push_back(object);
			testScene.AddObject(object.get());
		}
		
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

	return EXIT_SUCCESS;
}