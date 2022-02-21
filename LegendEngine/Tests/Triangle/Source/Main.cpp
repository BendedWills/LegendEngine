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
		InitScene(testScene);
		InitObject(testObject);

		testObject.AddComponent<Components::MeshComponent>();
		testScene.AddObject(testObject);

		SetActiveScene(testScene);

		return true;
	}

	void OnStop()
	{
		testScene.RemoveObject(testObject);
	}
private:
	Scene3D testScene;
	Object testObject;
};

int main()
{
	Triangle triangle;
	if (!triangle.Init("Triangle", true, true))
		return EXIT_FAILURE;
	
	Vulkan::VulkanRenderer renderer;
	if (!renderer.Init(&triangle))
		return EXIT_FAILURE;
	
	triangle.SetRenderer(&renderer);
	
	while (!triangle.IsCloseRequested())
	{
		triangle.Update();
		triangle.Render();
	}
	
	renderer.Dispose();
	triangle.Dispose();

	return EXIT_SUCCESS;
}