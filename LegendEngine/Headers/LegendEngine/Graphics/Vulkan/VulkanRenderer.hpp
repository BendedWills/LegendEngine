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
#include <LegendEngine/Graphics/Vulkan/Texture2DNative.hpp>
#include <LegendEngine/Graphics/Vulkan/MaterialNative.hpp>

#include <Tether/Tether.hpp>

#include <vk_mem_alloc.h>

namespace LegendEngine::Vulkan
{
	class VulkanRenderer : public IRenderer
	{
		// Why
		friend VertexBufferNative;
		// Are
		friend ShaderNative;
		// There
		friend ObjectNative;
		// So
		friend Texture2DNative;
		// Many
		friend MaterialNative;
		// Friend
		friend Pipeline;
		// Classes
		friend UniformBuffer;
	public:
		VulkanRenderer() {}
		
		VulkanRenderer(const VulkanRenderer&) = delete;
		VulkanRenderer(VulkanRenderer&&) = delete;
		VulkanRenderer& operator=(const VulkanRenderer&) = delete;
		VulkanRenderer& operator=(VulkanRenderer&&) = delete;
		
		void SetVSyncEnabled(bool vsync);
		bool CreateShaderNative(Resources::Shader* pShader);
		bool CreateTexture2DNative(Resources::Texture2D* pTexture);
		bool CreateMaterialNative(Resources::Material* pMaterial);

		/**
		 * @brief Reloads the renderer. Required after a settings change.
		 * 
		 * @returns True if the reload was successful; otherwise, false.
		 */
		bool Reload();
	protected:
		bool BeginSingleUseCommandBuffer(VkCommandBuffer* pCommandBuffer);
		bool EndSingleUseCommandBuffer(VkCommandBuffer commandBuffer);

		bool CreateImageView(VkImageView* pImageView, VkImage image, 
			VkFormat format, VkImageViewType viewType);

		bool CreateStagingBuffer(VkBuffer* pBuffer, VmaAllocation* pAllocation,
			VmaAllocationInfo* pAllocInfo, uint64_t size);

		bool ChangeImageLayout(VkImage image, VkFormat format, 
			VkImageLayout oldLayout, VkImageLayout newLayout);
		bool CopyBufferToImage(VkBuffer buffer, VkImage image, uint64_t width,
			uint64_t height);

		Vulkan::Surface surface;
		Vulkan::Device device;
		Vulkan::Swapchain swapchain;

		VkDescriptorSetLayout objectLayout;
		VkDescriptorSetLayout cameraLayout;
		VkDescriptorSetLayout materialLayout;
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

		const std::vector<const char*> deviceExtensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		Application* pApplication = nullptr;
		Vulkan::Instance* pInstance;

		Stopwatch timer;
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

		void OnWindowResize();

		void OnSceneObjectEnable(Scene* pScene, Objects::Object* pObject);
		void OnSceneObjectDisable(Scene* pScene, Objects::Object* pObject);

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



		bool shouldRecreateSwapchain = false;
		bool shouldRecreateCommandBuffers = false;
		bool enableVsync = false;
	};
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKANRENDERER_HPP