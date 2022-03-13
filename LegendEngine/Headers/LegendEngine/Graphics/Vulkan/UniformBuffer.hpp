/*
 * This header defines the surface. Surfaces in Vulkan are interfaces with
 * the window.
 */

#ifndef _LEGENDENGINE_VULKAN_UNIFORMBUFFER_HPP
#define _LEGENDENGINE_VULKAN_UNIFORMBUFFER_HPP
#ifdef VULKAN_API

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Graphics/Vulkan/Device.hpp>
#include <Tether/Tether.hpp>

#include <vulkan/vulkan.h>

#include <vk_mem_alloc.h>

namespace LegendEngine::Vulkan
{
    class VulkanRenderer;
    class UniformBuffer : public IDisposable
    {
        friend class Pipeline;
    public:
        UniformBuffer() {}
        LEGENDENGINE_NO_COPY(UniformBuffer);

        /**
         * @brief Initializes the UniformBuffer object.
         * 
         * @param pRenderer A pointer to the Vulkan renderer.
         * @param binding The binding in the shader of this uniform.
         * @param shaderStages The shader stages this uniform is accessible from.
         * @param images The number of swapchain images. This parameter must be the
         *  same as the images parameter in the PipelineInfo struct used to initialize
         *  the pipeline.
         * 
         * @returns True if initialization succeeded; otherwise, false.
         */
        bool Init(VulkanRenderer* pRenderer, uint64_t size,
            uint64_t binding, VkShaderStageFlags shaderStages, uint64_t images);

        /**
         * @brief Updates the uniform buffer. This uniform buffer must be passed
         *  to a pipeline class and that pipeline must be initialized before this
         *  function is used.
         */
        bool UpdateBuffer(void* newData, uint64_t currentImage);

        VkDescriptorSetLayoutBinding GetUboLayoutBinding();
        /**
         * @brief Gets the descriptor sets. This uniform buffer must be passed
         *  to a pipeline class and that pipeline must be initialized before this
         *  function is used.
         */
        std::vector<VkDescriptorSet>& GetDescriptorSets();
	protected:
        bool InitByPipeline(VkDescriptorSetLayout& layout, VkDescriptorPool* pDescPool);
    private:
        void OnDispose();

        VulkanRenderer* pRenderer = nullptr;
        
        VkDescriptorSetLayoutBinding binding{};

        std::vector<VkDescriptorSet> descriptorSets;

        std::vector<VmaAllocation> allocations;
        std::vector<VkBuffer> uniformBuffers;

        uint64_t size = 0;
        uint64_t images = 0;

        VkDescriptorPool* pDescriptorPool = nullptr;
        bool initByPipeline = false;
    };
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKAN_UNIFORMBUFFER_HPP
