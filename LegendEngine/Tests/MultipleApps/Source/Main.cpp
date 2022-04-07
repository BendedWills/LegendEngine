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
	Triangle triangle2;

	if (!triangle.Init("Triangle", true, true, RenderingAPI::VULKAN))
		return false;

	if (!triangle2.Init("Triangle 2", true, true, RenderingAPI::VULKAN))
		return false;
	
	while (!triangle.IsCloseRequested() || !triangle2.IsCloseRequested())
	{
		triangle.RenderFrame(1);
		triangle2.RenderFrame(1);
	}
	
	triangle.Dispose();
	triangle2.Dispose();

	Context::Dispose();
	return EXIT_SUCCESS;
}