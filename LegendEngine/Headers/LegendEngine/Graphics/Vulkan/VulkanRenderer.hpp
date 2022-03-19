#if !defined(VULKAN_API) && defined(__INTELLISENSE__)
#define VULKAN_API
#endif

#ifndef _LEGENDENGINE_VULKANRENDERER_HPP
#define _LEGENDENGINE_VULKANRENDERER_HPP
#ifdef VULKAN_API

#include <LegendEngine/Common/Stopwatch.hpp>
#include <LegendEngine/Graphics/IRenderer.hpp>
#include <LegendEngine/Graphics/Vulkan/Instance.hpp>
#include <LegendEngine/Graphics/Vulkan/Surface.hpp>
#include <LegendEngine/Graphics/Vulkan/Device.hpp>
#include <LegendEngine/Graphics/Vulkan/Swapchain.hpp>
#include <LegendEngine/Graphics/Vulkan/ShaderModule.hpp>
#include <LegendEngine/Graphics/Vulkan/Pipeline.hpp>
#include <LegendEngine/Graphics/Vulkan/UniformBuffer.hpp>

#include <vk_mem_alloc.h>

#include <Tether/Tether.hpp>

#include <iostream>
#include <vector>

namespace LegendEngine::Vulkan
{
	class VertexBuffer;
	class VulkanRenderer : public IRenderer
	{
		friend VertexBuffer;
		friend Pipeline;
		friend UniformBuffer;
	public:
		VulkanRenderer() 
			:
			eventHandler(this)
		{}
		
		VulkanRenderer(const VulkanRenderer&) = delete;
		VulkanRenderer(VulkanRenderer&&) = delete;
		VulkanRenderer& operator=(const VulkanRenderer&) = delete;
		VulkanRenderer& operator=(VulkanRenderer&&) = delete;
		
		void SetVSyncEnabled(bool vsync);
		bool CreateVertexBuffer(Ref<LegendEngine::VertexBuffer>* buffer);
		bool CreateShader(Ref<LegendEngine::Shader>* shader);

		/**
		 * @brief Reloads the renderer. Required after a settings change.
		 * 
		 * @returns True if the reload was successful; otherwise, false.
		 */
		bool Reload();
	protected:
		Vulkan::Surface surface;
		Vulkan::Device device;
		Vulkan::Swapchain swapchain;
		
		VkQueue graphicsQueue;
		VkQueue presentQueue;

		VkRenderPass renderPass;
		VkCommandPool commandPool;

		// Shader stuff
		Vulkan::UniformBuffer testUniform;
		Vulkan::ShaderModule vertexModule;
		Vulkan::ShaderModule fragmentModule;
		Vulkan::Pipeline shaderProgram;

		Vulkan::QueueFamilyIndices indices;
		VkPhysicalDevice physicalDevice;

		VmaAllocator allocator;

		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		std::vector<VkFence> imagesInFlight;
		std::vector<VkFramebuffer> framebuffers;
		std::vector<VkImage> swapchainImages;
		std::vector<VkImageView> swapchainImageViews;
		std::vector<VkCommandBuffer> commandBuffers;

		const int MAX_FRAMES_IN_FLIGHT = 2;
		uint64_t currentFrame = 0;

		class EventHandler : public Tether::Events::EventHandler
		{
		public:
			EventHandler(VulkanRenderer* pRenderer);
			void OnWindowResize(Tether::Events::WindowResizeEvent event);
		private:
			VulkanRenderer* pRenderer;
		};
		EventHandler eventHandler;

		const std::vector<const char*> deviceExtensions = 
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		
		Application* pApplication = nullptr;
		Vulkan::Instance* pInstance;

		Stopwatch timer;

		bool shouldRecreateSwapchain = false;
		bool enableVsync = false;
	private:
		bool RecreateSwapchain();

		bool RecreateCommandBuffers();
		bool PopulateCommandBuffer(VkCommandBuffer buffer, VkFramebuffer framebuffer,
			uint64_t commandBufferIndex);

		bool OnRendererInit();

		void OnSceneChange(Scene* pScene);
		void OnSceneObjectAdd(Scene* pScene, Objects::Object* pObject);
		void OnSceneObjectRemove(Scene* pScene, Objects::Object* pObject);
		void OnSceneRemove(Scene* pScene);
		void OnDefaultObjectAdd(Scene* pScene, Objects::Object* pObject);
		void OnDefaultObjectRemove(Scene* pScene, Objects::Object* pObject);
		void OnSceneObjectComponentAdd(
			Scene* pScene,
			Objects::Object* pObject,
			const std::string& typeName,
			Objects::Components::Component* pComponent
		);
		void OnSceneObjectComponentRemove(
			Scene* pScene,
			Objects::Object* pObject,
			const std::string& typeName,
			Objects::Components::Component* pComponent
		);

		bool OnRenderFrame();

		void OnRendererDispose();

		bool PickDevice(VkPhysicalDevice* pDevice, Vulkan::Surface* pSurface);
		bool IsDeviceSuitable(VkPhysicalDevice device, 
			Vulkan::Surface* pSurface);
		bool InitDevice();
		bool InitAllocator();
		VkSurfaceFormatKHR ChooseSurfaceFormat(Vulkan::SwapchainDetails details);
		bool InitSwapchain(uint64_t width, uint64_t height);
		bool InitShaders();
		bool InitRenderPass();
		bool InitPipeline();
		bool InitFramebuffers();
		bool InitCommandPool();
		bool InitCommandBuffers();
		bool InitSyncObjects();

		void UpdateUniforms(uint64_t imageIndex);

		bool DrawFrame();

		void DisposeSwapchain();
	};
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKANRENDERER_HPP