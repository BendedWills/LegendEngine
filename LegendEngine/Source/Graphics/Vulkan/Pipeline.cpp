#include <LegendEngine/Graphics/Vulkan/Pipeline.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>
#include <LegendEngine/Graphics/VertexBuffer.hpp>

#include <stdlib.h>

namespace LegendEngine::Vulkan
{
	Pipeline::Pipeline(
		TetherVulkan::GraphicsContext& context,
		VkExtent2D swapchainExtent,
		VkRenderPass renderPass,
		const PipelineInfo& pipelineInfo
	)
		:
		m_Device(context.GetDevice())
	{
		VkPipelineLayoutCreateInfo pipelineLayoutDesc{};
		pipelineLayoutDesc.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutDesc.setLayoutCount = pipelineInfo.setCount;
		pipelineLayoutDesc.pSetLayouts = pipelineInfo.pSetLayouts;

		if (vkCreatePipelineLayout(m_Device, &pipelineLayoutDesc,
			nullptr, &pipelineLayout) != VK_SUCCESS)
			throw std::runtime_error("Failed to create pipeline layout");

		std::vector<VkVertexInputBindingDescription> bindingDescs;
		std::vector<VkVertexInputAttributeDescription> attribDescs;

		// Vertex3c
		{
			VkVertexInputBindingDescription bindingDescription{};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(VertexTypes::Vertex3);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			VkVertexInputAttributeDescription position;
			position.location = 0;
			position.binding = 0;
			position.format = VK_FORMAT_R32G32B32_SFLOAT;
			position.offset = offsetof(VertexTypes::Vertex3, position);
			VkVertexInputAttributeDescription color;
			color.location = 1;
			color.binding = 0;
			color.format = VK_FORMAT_R32G32B32_SFLOAT;
			color.offset = offsetof(VertexTypes::Vertex3, texcoord);

			bindingDescs.push_back(bindingDescription);
			attribDescs.push_back(position);
			attribDescs.push_back(color);
		}

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount =
			static_cast<uint32_t>(bindingDescs.size());
		vertexInputInfo.pVertexBindingDescriptions = bindingDescs.data();
		vertexInputInfo.vertexAttributeDescriptionCount =
			static_cast<uint32_t>(attribDescs.size());
		vertexInputInfo.pVertexAttributeDescriptions = attribDescs.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

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
		rasterizer.cullMode = VK_CULL_MODE_NONE;
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

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_TRUE;
		depthStencil.depthWriteEnable = VK_TRUE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.minDepthBounds = 0.0f;
		depthStencil.maxDepthBounds = 1.0f;
		depthStencil.stencilTestEnable = VK_FALSE;
		depthStencil.front = {};
		depthStencil.back = {};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = pipelineInfo.dynamicStateCount;
		dynamicState.pDynamicStates = pipelineInfo.pDynamicStates;

		// Oh, yes, cool thing about Vulkan, you can actually have multiple shader
		// stages for one shader module. That means that you can have a VSMain and
		// a PSMain in one shader module (aka a glsl file in this case).

		VkGraphicsPipelineCreateInfo pipelineDesc{};
		pipelineDesc.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineDesc.stageCount = pipelineInfo.stageCount;
		pipelineDesc.pStages = pipelineInfo.pStages;
		pipelineDesc.pVertexInputState = &vertexInputInfo;
		pipelineDesc.pInputAssemblyState = &inputAssembly;
		pipelineDesc.pViewportState = &viewportState;
		pipelineDesc.pRasterizationState = &rasterizer;
		pipelineDesc.pMultisampleState = &multisampleState;
		pipelineDesc.pColorBlendState = &colorBlending;
		pipelineDesc.layout = pipelineLayout;
		pipelineDesc.renderPass = renderPass;
		pipelineDesc.subpass = 0;
		pipelineDesc.basePipelineHandle = VK_NULL_HANDLE;
		pipelineDesc.pDepthStencilState = &depthStencil;

		if (pipelineInfo.pDynamicStates && pipelineInfo.dynamicStateCount > 0)
			pipelineDesc.pDynamicState = &dynamicState;

		if (vkCreateGraphicsPipelines(m_Device, VK_NULL_HANDLE,
			1, &pipelineDesc, nullptr, &pipeline) != VK_SUCCESS)
			throw std::runtime_error("Failed to create graphics pipeline");
	}

	Pipeline::~Pipeline()
	{
		vkDestroyPipeline(m_Device, pipeline, nullptr);
		vkDestroyPipelineLayout(m_Device, pipelineLayout, nullptr);
	}

	VkPipeline Pipeline::GetPipeline()
	{
		return pipeline;
	}

	VkPipelineLayout Pipeline::GetPipelineLayout()
	{
		return pipelineLayout;
	}
}