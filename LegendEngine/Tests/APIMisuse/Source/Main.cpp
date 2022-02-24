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
		renderer.Dispose();
		renderer.Dispose();
		renderer.Dispose();
		renderer.Init(nullptr);
		SetRenderer(nullptr);

		InitScene(nullptr);
		InitObject(nullptr);

		testScene.AddObject(nullptr);
		testObject.AddComponent<Components::MeshComponent>()
			->Init(nullptr, 0);
		
		SetActiveScene(nullptr);

		Ref<VertexBuffer> buffer;
		//GetRenderer()->CreateVertexBuffer(nullptr);

		return true;
	}
	
	void OnStop()
	{
		testScene.RemoveObject(nullptr);
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
	if (!triangle.Start("Triangle", true, true))
		return EXIT_FAILURE;
	
	return EXIT_SUCCESS;
}