#ifndef _LEGENDENGINE_CONTEXT_HPP
#define _LEGENDENGINE_CONTEXT_HPP

#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Common/IDisposable.hpp>

#include <LegendEngine/Graphics/Vulkan/Instance.hpp>

namespace LegendEngine
{
	class ContextInternal : public IDisposable
	{
		friend class Context;
	protected:
		LEGENDENGINE_DISPOSE_ON_DESTRUCT(ContextInternal);

		// This class isn't meant to be construct by any other than Context
		ContextInternal() {}

		bool InitAPI(RenderingAPI api, bool debugMode);
	
		void OnDispose();

	#ifdef VULKAN_API
		bool InitVulkan(bool debugMode);
		bool vulkanInitialized = false;

		Vulkan::Instance instance;
	#endif // VULKAN_API
	#ifdef OPENGL_API
		// Not a thing yet
		// bool openglInitialized = false;

	#endif // OPENGL_API
	};

	class Context
	{
	public:
		static bool InitAPI(RenderingAPI api, bool debugMode);

	#ifdef VULKAN_API
		static Vulkan::Instance* GetVulkanInstance();
		static bool IsVulkanInitialized();
	#endif // VULKAN_API

		static void Dispose();
	private:
		inline static ContextInternal context;
	};
}

#endif //_LEGENDENGINE_CONTEXT_HPP