#include <LegendEngine/Common/Defs.hpp>

#include <LegendEngine/Graphics/Vulkan/ObjectNative.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>

#include <iostream>
#include <string.h>

namespace LegendEngine::Vulkan
{
	using namespace LegendEngine::Objects;

	ObjectNative::ObjectNative(TetherVulkan::GraphicsContext& context, 
		VkDescriptorSetLayout objectSetLayout, Object* pObject)
		:
		IObjectNative(pObject),
		m_Images(context.GetFramesInFlight()),
		m_Uniform(context, sizeof(ubo), m_Images),
		m_UniformManager(context, 1, m_Images)
	{
		m_Uniform.BindToSet(&m_UniformManager, objectSetLayout);
		m_Uniform.Bind(0);
	}

	void ObjectNative::SetCurrentImage(uint64_t imageIndex)
	{
		this->imageIndex = imageIndex;
	}

	void ObjectNative::OnUniformsUpdate()
	{
		ubo.transform = pObject->GetTransformationMatrix();
		m_Uniform.UpdateBuffer(&ubo, sizeof(ubo), imageIndex);
	}

	UniformBuffer* ObjectNative::GetUniform()
	{
		return &m_Uniform;
	}
}
