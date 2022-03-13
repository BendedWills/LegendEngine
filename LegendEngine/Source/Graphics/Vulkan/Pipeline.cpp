#ifdef VULKAN_API

#include <LegendEngine/Graphics/Vulkan/Pipeline.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Graphics/VertexBuffer.hpp>

#include <stdlib.h>

using namespace LegendEngine::Vulkan;

bool Pipeline::Init(
    VulkanRenderer* pRenderer,
    PipelineInfo* pPipelineInfo
)
{
    this->uniforms = pPipelineInfo->uniformCount > 0;

	if (initialized || !pPipelineInfo->pStages 
        || (!pPipelineInfo->ppUniforms && uniforms))
		return false;

    this->pRenderer = pRenderer;
    this->dynamicStates = dynamicStates;

    if (uniforms)
    {
        if (pPipelineInfo->images < 1)
            return false;

		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = pPipelineInfo->images 
            * pPipelineInfo->uniformCount;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = poolSize.descriptorCount;

        if (vkCreateDescriptorPool(pRenderer->device.Get(), &poolInfo, nullptr, 
            &descriptorPool) != VK_SUCCESS)
            return false;

		VkDescriptorSetLayoutBinding* bindings = (VkDescriptorSetLayoutBinding*)
			malloc(sizeof(VkDescriptorSetLayoutBinding) * pPipelineInfo->uniformCount);
		if (!bindings)
			return false;

        VkDescriptorSetLayoutCreateInfo dslCreateInfo{};
        dslCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        dslCreateInfo.bindingCount = pPipelineInfo->uniformCount;
        dslCreateInfo.pBindings = bindings;

        for (uint64_t i = 0; i < pPipelineInfo->uniformCount; i++)
            bindings[i] = pPipelineInfo->ppUniforms[i]->GetUboLayoutBinding();

        if (vkCreateDescriptorSetLayout(pRenderer->device.Get(), &dslCreateInfo,
            nullptr, &descriptorSetLayout) != VK_SUCCESS)
        {
            free(bindings);
            return false;
        }

        free(bindings);
        
		for (uint64_t uniform = 0; uniform < pPipelineInfo->uniformCount; uniform++)
            if (!pPipelineInfo->ppUniforms[uniform]->InitByPipeline(
                descriptorSetLayout, &descriptorPool))
                return false;
    }

    if (!InitPipeline(pPipelineInfo))
    {
        if (uniforms)
			vkDestroyDescriptorSetLayout(pRenderer->device.Get(), 
                descriptorSetLayout, nullptr);

        return false;
    }

    initialized = true;
    return true;
}

VkPipeline Pipeline::GetPipeline()
{
    return pipeline;
}

VkPipelineLayout Pipeline::GetPipelineLayout()
{
	return pipelineLayout;
}

bool Pipeline::InitPipeline(
    PipelineInfo* pPipelineInfo
)
{
    VkPipelineLayoutCreateInfo pipelineLayoutDesc{};
    pipelineLayoutDesc.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    
    if (uniforms)
    {
        pipelineLayoutDesc.setLayoutCount = 1;
        pipelineLayoutDesc.pSetLayouts = &descriptorSetLayout;
    }

    if (vkCreatePipelineLayout(pRenderer->device.Get(), &pipelineLayoutDesc,
        nullptr, &pipelineLayout) != VK_SUCCESS)
        return false;

    std::vector<VkVertexInputBindingDescription> bindingDescs;
    std::vector<VkVertexInputAttributeDescription> attribDescs;

    // Vertex2
    {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(VertexTypes::Vertex2);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        VkVertexInputAttributeDescription position;
        position.location = 0;
        position.binding = 0;
        position.format = VK_FORMAT_R32G32_SFLOAT;
        position.offset = offsetof(VertexTypes::Vertex2, position);
        VkVertexInputAttributeDescription color;
        color.location = 1;
        color.binding = 0;
        color.format = VK_FORMAT_R32G32B32_SFLOAT;
        color.offset = offsetof(VertexTypes::Vertex2, color);

        bindingDescs.push_back(bindingDescription);
        attribDescs.push_back(position);
        attribDescs.push_back(color);
    }

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = bindingDescs.size();
    vertexInputInfo.pVertexBindingDescriptions = bindingDescs.data();
    vertexInputInfo.vertexAttributeDescriptionCount = attribDescs.size();
    vertexInputInfo.pVertexAttributeDescriptions = attribDescs.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkExtent2D swapchainExtent = pRenderer->swapchain.GetExtent();

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)swapchainExtent.width;
    viewport.height = (float)swapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = swapchainExtent.width;
    scissor.extent.height = swapchainExtent.height;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampleState{};
    multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleState.sampleShadingEnable = VK_FALSE;
    multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleState.minSampleShading = 0.0f;
    multisampleState.pSampleMask = nullptr;
    multisampleState.alphaToCoverageEnable = VK_FALSE;
    multisampleState.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = pPipelineInfo->dynamicStateCount;
    dynamicState.pDynamicStates = pPipelineInfo->pDynamicStates;

    // Oh, yes, cool thing about Vulkan, you can actually have multiple shader
    // stages for one shader module. That means that you can have a VSMain and
    // a PSMain in one shader module (aka a glsl file in this case).
    
    VkGraphicsPipelineCreateInfo pipelineDesc{};
    pipelineDesc.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineDesc.stageCount = pPipelineInfo->stageCount;
    pipelineDesc.pStages = pPipelineInfo->pStages;
    pipelineDesc.pVertexInputState = &vertexInputInfo;
    pipelineDesc.pInputAssemblyState = &inputAssembly;
    pipelineDesc.pViewportState = &viewportState;
    pipelineDesc.pRasterizationState = &rasterizer;
    pipelineDesc.pMultisampleState = &multisampleState;
    pipelineDesc.pColorBlendState = &colorBlending;
    pipelineDesc.layout = pipelineLayout;
    pipelineDesc.renderPass = pRenderer->renderPass;
    pipelineDesc.subpass = 0;
    pipelineDesc.basePipelineHandle = VK_NULL_HANDLE;

    if (pPipelineInfo->pDynamicStates && pPipelineInfo->dynamicStateCount > 0)
        pipelineDesc.pDynamicState = &dynamicState;

    return vkCreateGraphicsPipelines(pRenderer->device.Get(), VK_NULL_HANDLE,
        1, &pipelineDesc, nullptr, &pipeline) == VK_SUCCESS;
}

void Pipeline::OnDispose()
{
    vkDestroyPipeline(pRenderer->device.Get(), pipeline, nullptr);
    vkDestroyPipelineLayout(pRenderer->device.Get(), pipelineLayout, nullptr);
    
    if (uniforms)
    {
        vkDestroyDescriptorPool(pRenderer->device.Get(), descriptorPool, nullptr);
        vkDestroyDescriptorSetLayout(pRenderer->device.Get(), descriptorSetLayout, 
            nullptr);
    }

    uniforms = false;
    dynamicStates = false;
}

#endif // VULKAN_API