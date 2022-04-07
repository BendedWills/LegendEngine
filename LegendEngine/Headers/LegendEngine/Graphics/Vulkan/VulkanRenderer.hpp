#ifndef _LEGENDENGINE_VULKANRENDERER_HPP
#define _LEGENDENGINE_VULKANRENDERER_HPP

#include <LegendEngine/Common/Defs.hpp>
#ifdef VULKAN_API

#include <iostream>
#include <vector>

#include <LegendEngine/Common/Stopwatch.hpp>
#include <LegendEngine/Objects/Object.hpp>
#include <LegendEngine/Objects/Camera.hpp>
#include <LegendEngine/Graphics/IRenderer.hpp>
#include <LegendEngine/Graphics/Vulkan/Instance.hpp>
#include <LegendEngine/Graphics/Vulkan/Surface.hpp>
#include <LegendEngine/Graphics/Vulkan/Device.hpp>
#include <LegendEngine/Graphics/Vulkan/Swapchain.hpp>
#include <LegendEngine/Graphics/Vulkan/ShaderModule.hpp>
#include <LegendEngine/Graphics/Vulkan/Pipeline.hpp>
#include <LegendEngine/Graphics/Vulkan/UniformBuffer.hpp>

#include <LegendEngine/Graphics/Vulkan/VertexBufferNative.hpp>
#include <LegendEngine/Graphics/Vulkan/ShaderNative.hpp>
#include <LegendEngine/Graphics/Vulkan/ObjectNative.hpp>

#include <Tether/Tether.hpp>

#include <vk_mem_alloc.h>

namespace LegendEngine::Vulkan
{
	class VulkanRenderer : public IRenderer
	{
		friend VertexBufferNative;
		friend ShaderNative;
		friend ObjectNative;
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
		bool CreateShaderNative(Shader* shader);

		/**
		 * @brief Reloads the renderer. Required after a settings change.
		 * 
		 * @returns True if the reload was successful; otherwise, false.
		 */
		bool Reload();
		
		Vulkan::Surface surface;
		Vulkan::Device device;
		Vulkan::Swapchain swapchain;

		VkDescriptorSetLayout objectLayout;
		VkDescriptorSetLayout cameraLayout;
		Vulkan::UniformManager cameraManager;
		Vulkan::UniformBuffer cameraUniform;
		
		VkQueue graphicsQueue;
		VkQueue presentQueue;

		VkRenderPass renderPass;
		VkCommandPool commandPool;

		// Shader stuff
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
		bool CreateObjectNative(Objects::Object* pObject);
		bool CreateVertexBufferNative(LegendEngine::VertexBuffer* buffer);

		bool RecreateSwapchain();

		bool RecreateCommandBuffers();
		bool PopulateCommandBuffer(VkCommandBuffer buffer, VkFramebuffer framebuffer,
			uint64_t commandBufferIndex);
		void PopulateByScene(VkCommandBuffer buffer, VkFramebuffer framebuffer,
			uint64_t commandBufferIndex, Scene* pScene);

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
		bool InitUniforms();
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