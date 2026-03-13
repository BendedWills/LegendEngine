#include "LegendEngine/Graphics/GraphicsContext.hpp"

#ifdef _VULKAN_API
#include <LegendEngine/Graphics/Vulkan/VulkanGraphicsContext.hpp>
#endif

namespace le
{
    GraphicsContext::~GraphicsContext() {}

    Scope<GraphicsContext> GraphicsContext::Create(const GraphicsAPI api,
        std::string_view applicationName)
    {
        switch (api)
        {
#ifdef _VULKAN_API
            case GraphicsAPI::VULKAN: return std::make_unique<VulkanGraphicsContext>(
                applicationName);
#endif
        }

        throw std::runtime_error("Graphics API not implemented");
    }
}
