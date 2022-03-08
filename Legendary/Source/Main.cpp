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
		if (!renderer.Init(this))
			return false;
		SetRenderer(&renderer);

		InitScene(testScene);
		InitObject(testObject);

		VertexTypes::Vertex2 testVertices[] =
		{
			{   0.0f, -0.5f },
			{   0.5f,  0.5f },
			{  -0.5f,  0.5f }
		};

		testObject.AddComponent<Components::MeshComponent>()
			->Init(testVertices, 3);
		testScene.AddObject(testObject);

		SetActiveScene(testScene);

		return true;
	}

	void OnStop()
	{
		testScene.RemoveObject(testObject);
		renderer.Dispose();
	}
private:
	Scene testScene;
	Object testObject;

	Vulkan::VulkanRenderer renderer;
};

int main()
{
	Triangle triangle;
	if (!triangle.Start("Legendary", true, true))
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}