#include <LE/Graphics/GraphicsContext.hpp>
#include <LE/Common/Assert.hpp>

#define LE_GRAPHICS_CTX_CREATE_FUNC(name) \
    __attribute__((weak)) \
    Scope<GraphicsContext> Create##name##GraphicsContext(std::string_view) \
    { \
        LE_ASSERT(false, "Create"#name"GraphicsContext not overridden. Was the program linked with LE"#name"?"); \
        return nullptr; \
    }

namespace le
{
    LE_GRAPHICS_CTX_CREATE_FUNC(Vulkan);
    LE_GRAPHICS_CTX_CREATE_FUNC(OpenGL);
    LE_GRAPHICS_CTX_CREATE_FUNC(D3D11);
    LE_GRAPHICS_CTX_CREATE_FUNC(D3D12);
    LE_GRAPHICS_CTX_CREATE_FUNC(WebGPU);

    GraphicsContext::~GraphicsContext() = default;

    Scope<GraphicsContext> GraphicsContext::Create(const GraphicsAPI api,
        const std::string_view applicationName)
    {
        switch (api)
        {
            case GraphicsAPI::VULKAN: return CreateVulkanGraphicsContext(applicationName);
            case GraphicsAPI::OPENGL: return CreateOpenGLGraphicsContext(applicationName);
            case GraphicsAPI::D3D11:  return CreateD3D11GraphicsContext(applicationName);
            case GraphicsAPI::D3D12:  return CreateD3D12GraphicsContext(applicationName);
            case GraphicsAPI::WEBGPU: return CreateWebGPUGraphicsContext(applicationName);
        }

        LE_ASSERT(false, "Unknown graphics API");
        return nullptr;
    }
}
