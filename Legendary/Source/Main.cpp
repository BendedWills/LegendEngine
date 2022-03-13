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
		//renderer.SetVSyncEnabled(true);
		SetRenderer(&renderer);

		InitScene(testScene);
		
		srand(time(NULL));
		for (uint64_t i = 0; i < 100; i++)
		{
			float offsetX = (float)rand() / RAND_MAX * 2 - 1;
			float offsetY = (float)rand() / RAND_MAX * 2 - 1;

			float r = (float)rand() / RAND_MAX;
			float g = (float)rand() / RAND_MAX;
			float b = (float)rand() / RAND_MAX;

			VertexTypes::Vertex2 testVertices[] =
			{
				{   0.0f  + offsetX, -0.05f + offsetY, r, g, b, },
				{   0.05f + offsetX,  0.05f + offsetY, r, g, b, },
				{  -0.05f + offsetX,  0.05f + offsetY, r, g, b, }
			};

			Ref<Object> object = RefTools::Create<Object>();
			InitObject(object.get());

			object->AddComponent<Components::MeshComponent>()->Init(
				testVertices, 3
			);

			objects.push_back(object);

			testScene.AddObject(object.get());
		}

		SetActiveScene(testScene);

		return true;
	}

	void OnStop()
	{
		renderer.Dispose();
		testScene.ClearObjects();
	}
private:
	Scene testScene;
	
	std::vector<Ref<Object>> objects;
	Vulkan::VulkanRenderer renderer;
};

int main()
{
	{
		Triangle triangle;
		if (!triangle.Start("Legendary", true, true))
			return EXIT_FAILURE;
	}
	
	return EXIT_SUCCESS;
}