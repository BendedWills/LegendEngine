#pragma once

#include <LegendEngine/Graphics/VertexBuffer.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanGraphicsContext.hpp>

#include <Tether/Rendering/Vulkan/BufferStager.hpp>

#include <span>
#include <vulkan/vulkan.h>

namespace le
{
    class VulkanVertexBuffer : public VertexBuffer
    {
    public:
        VulkanVertexBuffer(
            VulkanGraphicsContext& context,
            size_t initialVertexCount, size_t initialIndexCount
        );
        ~VulkanVertexBuffer() override;

        void Update(std::span<VertexTypes::Vertex3> vertices, std::span<uint32_t> indices) override;
        void Resize(size_t vertexCount, size_t indexCount) override;
        size_t GetVertexCount() override;
        size_t GetIndexCount() override;

        [[nodiscard]] VkBuffer GetVertexBuffer() const;
        [[nodiscard]] VkBuffer GetIndexBuffer() const;

        void Wait() const;

        // This function frees the staging buffers, so that less memory is used.
        // Once this is called, data can't be uploaded anymore.
        void FinishDataUpload();
    private:
        void CreateVertexBuffer(size_t size);
        void CreateIndexBuffer(size_t size);

        void DestroyBuffers();

        size_t m_VertexCount = 0;
        size_t m_IndexCount = 0;

        bool m_FinishedUploading = true;

        VkBuffer m_VertexBuffer = nullptr;
        VmaAllocation m_VertexAllocation = nullptr;
        VkBuffer m_IndexBuffer = nullptr;
        VmaAllocation m_IndexAllocation = nullptr;

        Tether::Rendering::Vulkan::GraphicsContext& m_Context;

        std::mutex& m_Mutex;

        std::optional<Tether::Rendering::Vulkan::BufferStager> m_VertexStager;
        std::optional<Tether::Rendering::Vulkan::BufferStager> m_IndexStager;
    };
}
