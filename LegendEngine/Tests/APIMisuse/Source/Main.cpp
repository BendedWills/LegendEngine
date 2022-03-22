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
		testObject = CreateObject<Object>();

		VertexTypes::Vertex2 testVertices[] =
		{
			{   0.0f,  -0.05f, 1, 0, 0, },
			{   0.05f,  0.05f, 0, 1, 0, },
			{  -0.05f,  0.05f, 0, 0, 1, }
		};

		testScene.AddObject(testObject);
		testObject->AddComponent<Components::MeshComponent>()
			->Init(testVertices, 3);

		SetActiveScene(testScene);

		return true;
	}

	void OnStop()
	{
		testScene.RemoveObject(testObject);
	}
private:
	Scene testScene;
	Ref<Object> testObject;
};

int main()
{
	Context::InitAPI(RenderingAPI::VULKAN, true);

	Triangle triangle;
	if (!triangle.Init("Triangle", true, true, RenderingAPI::VULKAN))
		return false;

	while (!triangle.IsCloseRequested())
	{
		triangle.RenderFrame();
	}

	triangle.Dispose();

	Context::Dispose();
	return EXIT_SUCCESS;
}