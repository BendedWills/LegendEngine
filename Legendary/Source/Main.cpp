#include <iostream>
#include <chrono>
#include <thread>

#include <LegendEngine/LegendEngine.hpp>

using namespace LegendEngine;
using namespace LegendEngine::Objects;
using namespace std::literals::chrono_literals;

class Triangle : public Application
{
public:
	bool OnInit()
	{
		InitScene(testScene);
		
		srand(static_cast<unsigned int>(time(NULL)));
		for (uint64_t i = 0; i < 1000; i++)
		{
			float r = (float)rand() / RAND_MAX;
			float g = (float)rand() / RAND_MAX;
			float b = (float)rand() / RAND_MAX;

			VertexTypes::Vertex2 testVertices[] =
			{
				{   0.0f,  -0.05f, r, g, b, },
				{   0.05f,  0.05f, r, g, b, },
				{  -0.05f,  0.05f, r, g, b, }
			};

			Ref<Object> object = CreateObject<Object>();

			object->AddComponent<Components::MeshComponent>()->Init(
				testVertices, 3
			);

			objects.push_back(object);
			testScene.AddObject(object.get());
		}

		SetActiveScene(testScene);

		return true;
	}

	void OnUpdate()
	{
		for (uint64_t i = 0; i < objects.size(); i++)
		{
			float offsetX = (float)rand() / RAND_MAX * 2 - 1;
			float offsetY = (float)rand() / RAND_MAX * 2 - 1;

			objects[i]->SetPosition(Vector3f(offsetX, offsetY, 0));
		}
	}

	void OnStop()
	{
		testScene.ClearObjects();
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

		while (!triangle.IsCloseRequested())
			triangle.RenderFrame();

		triangle.Dispose();
	}
	Context::Dispose();
	
	return EXIT_SUCCESS;
}