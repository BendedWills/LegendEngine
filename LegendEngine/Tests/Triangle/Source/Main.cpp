#include <iostream>

#include <LegendEngine/Application3D.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>

using namespace LegendEngine;

class Triangle : public Application3D
{
public:
	bool OnInit()
	{
		return false;
	}
};

int main()
{
	Triangle triangle;
	if (!triangle.Init("Triangle", true, true))
		return EXIT_FAILURE;
	triangle.SetFullscreen(true);
	
	Vulkan::VulkanRenderer renderer;
	if (!renderer.Init(&triangle))
		return EXIT_FAILURE;
	
	triangle.SetRenderer(&renderer);

	while (!triangle.IsCloseRequested())
	{
		triangle.ProcessFrame();
	}
	
	renderer.Dispose();
	triangle.Dispose();

	return EXIT_SUCCESS;
}