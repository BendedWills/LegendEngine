#pragma once

#include <LegendEngine/GraphicsContext.hpp>

#include <Tether/Rendering/Vulkan/Instance.hpp>

namespace LegendEngine::Vulkan
{
	namespace TetherVulkan = Tether::Rendering::Vulkan;

	class GraphicsContext : public LegendEngine::GraphicsContext
	{
	public:
		using Instance = TetherVulkan::Instance;

		GraphicsContext(std::string_view applicationName, bool debug);

		TetherVulkan::GraphicsContext& GetTetherGraphicsContext();
	private:
		TetherVulkan::InstanceInfo CreateInstanceInfo(const char* appName);

		TetherVulkan::ContextCreator m_ContextCreator;
		TetherVulkan::GraphicsContext m_GraphicsContext;
	};
}