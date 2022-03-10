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
		SetActiveScene(testScene);
		
		srand(time(NULL));
		for (uint64_t i = 0; i < 100; i++)
		{
			float offsetX = (rand() % 65536) / 65536.0f * 4 - 1;
			float offsetY = (rand() % 65536) / 65536.0f * 4 - 1;

			VertexTypes::Vertex2 testVertices[] =
			{
				{   0.0f + offsetX, -0.1f + offsetY },
				{   0.1f + offsetX,  0.1f + offsetY },
				{  -0.1f + offsetX,  0.1f + offsetY }
			};

			Ref<Object> object = RefTools::Create<Object>();
			InitObject(object.get());

			object->AddComponent<Components::MeshComponent>()->Init(
				testVertices, 3
			);

			objects.push_back(object);

			testScene.AddObject(object.get());
		}

		for (uint64_t i = 0; i < 50; i++)
		{
			Ref<Object> object = objects[i];
			object->RemoveComponent<Components::MeshComponent>();
		}

		return true;
	}

	void OnStop()
	{
		for (uint64_t i = 0; i < 100; i++)
			testScene.RemoveObject(objects[i].get());
		renderer.Dispose();
	}
private:
	Scene testScene;
	
	std::vector<Ref<Object>> objects;
	Vulkan::VulkanRenderer renderer;
};

int main()
{
	Triangle triangle;
	if (!triangle.Start("Legendary", true, true))
		return EXIT_FAILURE;

	return EXIT_SUCCESS;
}