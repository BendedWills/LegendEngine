#include <iostream>
#include <chrono>
#include <thread>

#include <LegendEngine/LegendEngine.hpp>

using namespace LegendEngine;
using namespace LegendEngine::Object3d;
using namespace std::literals::chrono_literals;

class Triangle : public Application3D
{
public:
	bool OnInit()
	{
		if (!renderer.Init(this))
			return false;
		SetRenderer(&renderer);

		InitScene(testScene);
		InitObject(testObject);

		VertexTypes::Vertex2 testVertices[] =
		{
			{  0.5f, -0.5f },
			{ -0.5f, -0.5f },
			{  0.0f,  0.5f }
		};

		testScene.AddObject(testObject);
		testObject.AddComponent<Components::MeshComponent>()
			->Init(testVertices, 3);
		
		SetActiveScene(testScene);
		
		return true;
	}
	
	void OnStop()
	{
		testScene.RemoveObject(testObject);
		renderer.Dispose();
	}
private:
	Scene3D testScene;
	Object testObject;

	Vulkan::VulkanRenderer renderer;
};

int main()
{
	Triangle triangle;
	Triangle triangle2;

	std::thread triangle2thread(
	[&]() 
	{
		if (!triangle2.Start("Triangle 2", true, true))
			return;
	});

	if (!triangle.Start("Triangle", true, true))
		return EXIT_FAILURE;
	
	triangle2thread.join();
	
	return EXIT_SUCCESS;
}