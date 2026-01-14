#pragma once

#include <LegendEngine/Common/Defs.hpp>

#include <iostream>
#include <vector>

#include <LegendEngine/Common/Stopwatch.hpp>
#include <LegendEngine/Objects/Object.hpp>
#include <LegendEngine/Objects/Camera.hpp>
#include <LegendEngine/Graphics/IRenderer.hpp>
#include <LegendEngine/Graphics/Vulkan/GraphicsContextVk.hpp>
#include <LegendEngine/Graphics/Vulkan/ShaderModule.hpp>
#include <LegendEngine/Graphics/Vulkan/Pipeline.hpp>
#include <LegendEngine/Graphics/Vulkan/UniformBuffer.hpp>

#include <LegendEngine/Graphics/Vulkan/VertexBufferNative.hpp>
#include <LegendEngine/Graphics/Vulkan/ShaderNative.hpp>
#include <LegendEngine/Graphics/Vulkan/ObjectNative.hpp>
#include <LegendEngine/Graphics/Vulkan/Texture2DNative.hpp>
#include <LegendEngine/Graphics/Vulkan/MaterialNative.hpp>

#include <Tether/Tether.hpp>
#include <Tether/Rendering/Vulkan/Device.hpp>
#include <Tether/Rendering/Vulkan/Swapchain.hpp>

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
		VulkanRenderer(Application& application, Tether::Window& window);
		~VulkanRenderer() override;
		
		VulkanRenderer(const VulkanRenderer&) = delete;
		VulkanRenderer(VulkanRenderer&&) = delete;
		VulkanRenderer& operator=(const VulkanRenderer&) = delete;
		VulkanRenderer& operator=(VulkanRenderer&&) = delete;
		
		// Setting changing
		void SetVSyncEnabled(bool vsync) override;

		// Native creation
		bool CreateObjectNative(Objects::Object* pObject) override;
		bool CreateVertexBufferNative(LegendEngine::VertexBuffer* buffer) override;
		bool CreateShaderNative(Resources::Shader* pShader,
			std::optional<std::span<Resources::ShaderStage>> stages) override;
		bool CreateTexture2DNative(Resources::Texture2D* pTexture) override;
		bool CreateMaterialNative(Resources::Material* pMaterial) override;
	protected:
		// Vulkan utility functions
		bool BeginSingleUseCommandBuffer(VkCommandBuffer* pCommandBuffer) const;
		bool EndSingleUseCommandBuffer(VkCommandBuffer commandBuffer) const;
		void CreateImageView(VkImageView* pImageView, VkImage image, 
			VkFormat format, VkImageViewType viewType, VkImageAspectFlags aspectFlags) const;
		bool CreateStagingBuffer(VkBuffer* pBuffer, VmaAllocation* pAllocation,
			VmaAllocationInfo* pAllocInfo, uint64_t size) const;
		void ChangeImageLayout(VkImage image, VkFormat format, 
			VkImageLayout oldLayout, VkImageLayout newLayout) const;
		bool CopyBufferToImage(VkBuffer buffer, VkImage image, uint64_t width,
			uint64_t height) const;
		[[nodiscard]] VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates,
			VkImageTiling tiling, VkFormatFeatureFlags features) const;
		[[nodiscard]] VkFormat FindDepthFormat() const;
		TetherVulkan::SwapchainDetails QuerySwapchainSupport();

		Application& m_Application;
		Tether::Window& m_Window;

		TetherVulkan::GraphicsContext& m_GraphicsContext;

		VkInstance m_Instance;
		VkDevice m_Device;
		VkPhysicalDevice m_PhysicalDevice;
		VkQueue m_Queue = nullptr;

		TetherVulkan::Surface m_Surface;

		std::optional<TetherVulkan::Swapchain> m_Swapchain;

		// Descriptor set stuff
		VkDescriptorSetLayout objectLayout{};
		VkDescriptorSetLayout cameraLayout{};
		VkDescriptorSetLayout materialLayout{};
		std::optional<Vulkan::UniformManager> uniformManager;
		std::optional<Vulkan::UniformBuffer> cameraUniform;

		VkRenderPass renderPass{};
		VkCommandPool m_CommandPool = nullptr;

		// Shader stuff
		Ref<Resources::Shader> solidShader;
		Ref<Resources::Shader> texturedShader;

		VmaAllocator m_Allocator = nullptr;

		VkImage depthImage{};
		VmaAllocation depthAlloc{};
		VkImageView depthImageView{};

		std::vector<VkFramebuffer> framebuffers;
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;
		std::vector<VkImage> swapchainImages;
		std::vector<VkImageView> swapchainImageViews;
		std::vector<VkCommandBuffer> commandBuffers;

		VkSurfaceFormatKHR m_SurfaceFormat{};

		const int MAX_FRAMES_IN_FLIGHT = 2;
		uint64_t currentFrame = 0;

		const std::vector<const char*> deviceExtensions =
		{
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		Stopwatch timer;
	private:
		bool RecreateSwapchain();

		// Command buffers
		bool RecreateCommandBuffers(uint32_t imageIndex);
		bool PopulateCommandBuffer(VkCommandBuffer buffer, VkFramebuffer framebuffer,
			uint64_t commandBufferIndex);
		void PopulateByScene(VkCommandBuffer buffer, VkFramebuffer framebuffer,
			uint64_t commandBufferIndex, Scene* pScene);

		void CreateShaders() override;

		// Renderer virtual functions
		void OnWindowResize();
		bool OnRenderFrame() override;

		// Device helper functions
		void ChooseSurfaceFormat(const TetherVulkan::SwapchainDetails& details);
		
		// Init functions
		void InitSwapchain();
		void InitRenderPass();
		void InitUniforms();
		void InitDepthImages();
		void InitFramebuffers();
		void InitCommandBuffers();
		void InitSyncObjects();

		void CreateDefaultMaterialUniforms();

		static void UpdateDefaultMaterialUniforms();

		// Uniforms
		void UpdateUniforms(uint64_t imageIndex);

		static void UpdateSceneUniforms(uint64_t imageIndex, Scene* pScene);

		bool DrawFrame();

		// Disposal functions (lol there's just one)
		void DisposeSwapchain();

		VkDescriptorSet m_Sets[3] = {};

		bool shouldRecreateSwapchain = false;
		bool enableVsync = false;

		std::optional<Vulkan::UniformBuffer> m_DefaultMatUniform;
		VkDescriptorPool pool{};
	};
}
