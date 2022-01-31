#include <iostream>

#include <LegendEngine/Application.hpp>
#include <LegendEngine/VulkanRenderer.hpp>

using namespace LegendEngine;

class Triangle : public Application
{
public:
	bool OnInit()
	{
		renderer.SetValidationLayersEnabled(true);
		renderer.Init(Get());
		SetRenderer(&renderer);

		return false;
	}

	void OnStop()
	{
		renderer.Dispose();
	}
private:
	VulkanRenderer renderer;
};

int main()
{
	{
		Triangle triangle;
		if (!triangle.Init("Triangle", true, true))
			return EXIT_FAILURE;
		
		while (!triangle.IsCloseRequested())
		{
			triangle.ProcessFrame();
		}
		
		triangle.Dispose();
	}

	return EXIT_SUCCESS;
}