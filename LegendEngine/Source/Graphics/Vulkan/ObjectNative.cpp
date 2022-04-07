#include <LegendEngine/Common/Defs.hpp>
#ifdef VULKAN_API

#include <LegendEngine/Graphics/Vulkan/ObjectNative.hpp>
#include <LegendEngine/Graphics/Vulkan/VulkanRenderer.hpp>

#include <iostream>
#include <string.h>

using namespace LegendEngine::Vulkan;
using namespace LegendEngine::Objects;

ObjectNative::ObjectNative(VulkanRenderer* pRenderer, Object* pObject)
	:
	IObjectNative(pObject)
{
	this->pRenderer = pRenderer;
	this->images = pRenderer->swapchain.GetImageCount();

	if (!uniform.Init(pRenderer, sizeof(ubo), images))
		return;
	if (!uniformManager.Init(&pRenderer->device, 1, images))
		return;

	uniform.BindToSet(&uniformManager, pRenderer->objectLayout);
	uniform.Bind(0);
}

void ObjectNative::SetCurrentImage(uint64_t imageIndex)
{
	this->imageIndex = imageIndex;
}

void ObjectNative::OnUniformsUpdate()
{
	ubo.transform = pObject->GetTransformationMatrix();
	uniform.UpdateBuffer(&ubo, sizeof(ubo), imageIndex);
}

UniformBuffer* ObjectNative::GetUniform()
{
	return &uniform;
}

void ObjectNative::OnDispose()
{
	pRenderer->device.WaitIdle();

	uniform.Dispose();
	uniformManager.Dispose();
}

#endif // VULKAN_API