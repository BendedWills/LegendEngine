#pragma once
#include <VulkanTextureBridge.hpp>
#include <LE/IO/TextureData.hpp>
#include <LE/Resources/Texture2DArray.hpp>

namespace le
{
	class VulkanTexture2DArray final
	{
	public:
		VulkanTexture2DArray(
			Tether::Rendering::Vulkan::GraphicsContext& context,
			size_t width, size_t height, uint8_t channels,
			const std::span<TextureData*>& textureData, std::mutex& mutex
		);
		~VulkanTexture2DArray() override;

		Texture::APIBridge* GetAPIBridge() override;
	private:
		void CreateImage(VkFormat format, size_t arrayLayers,
			size_t width, size_t height);
		void CreateImageView(VkFormat format, size_t arrayLayers);
		void CreateSampler();
		void UploadData(uint32_t width, uint32_t height, size_t size,
			const std::span<TextureData*>& textureData, std::mutex& mutex) const;
		static void CopyImagesToBuffer(uint8_t* data, size_t size,
			const std::span<TextureData*>& textureData);

		Tether::Rendering::Vulkan::GraphicsContext& m_Context;

		VkImage m_Image = nullptr;
		VkImageView m_ImageView = nullptr;
		VkSampler m_Sampler = nullptr;
		VmaAllocation m_ImageAllocation = nullptr;
	};
}
