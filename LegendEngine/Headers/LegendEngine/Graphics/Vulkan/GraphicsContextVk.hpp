#pragma once

#include <LegendEngine/GraphicsContext.hpp>

#include <Tether/Rendering/Vulkan/ContextCreator.hpp>

// Don't Remove these. They are used by other files.
#include <Tether/Rendering/Vulkan/ImageStager.hpp>
#include <Tether/Rendering/Vulkan/BufferStager.hpp>

namespace LegendEngine::Vulkan
{
	namespace TetherVulkan = Tether::Rendering::Vulkan;

	class GraphicsContext : public LegendEngine::GraphicsContext
	{
	public:
		using Instance = TetherVulkan::Instance;

		GraphicsContext(std::string_view applicationName, bool debug);
		~GraphicsContext() override;
		
		TetherVulkan::GraphicsContext& GetTetherGraphicsContext();
	private:
		Scope<IRenderer> CreateRenderer(Application& app, Tether::Window& window) override;

		class DebugCallback : public Vulkan::TetherVulkan::DebugCallback
		{
		public:
			DebugCallback(GraphicsContext& context);

			void OnDebugLog(
				VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
				VkDebugUtilsMessageTypeFlagsEXT messageType,
				const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData
			);
		private:
			GraphicsContext& m_Context;
		};

		TetherVulkan::InstanceInfo CreateInstanceInfo(const char* appName);

		TetherVulkan::ContextCreator m_ContextCreator;
		TetherVulkan::GraphicsContext m_GraphicsContext;
		
		DebugCallback m_Callback;
	};
}