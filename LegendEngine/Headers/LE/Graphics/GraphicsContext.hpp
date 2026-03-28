#pragma once

#include <memory>
#include <LE/Common/Defs.hpp>
#include <LE/Common/Types.hpp>
#include <LE/Graphics/Renderer.hpp>
#include <LE/Graphics/RenderTarget.hpp>
#include <LE/Graphics/ShaderManager.hpp>
#include <LE/Graphics/API/Buffer.hpp>
#include <LE/Graphics/API/CommandBuffer.hpp>
#include <LE/Graphics/API/DescriptorSetLayout.hpp>
#include <LE/Graphics/API/DynamicUniforms.hpp>
#include <LE/Graphics/API/Image.hpp>
#include <LE/Graphics/API/Pipeline.hpp>
#include <LE/Graphics/API/Sampler.hpp>
#include <LE/Resources/Shader.hpp>
#include <Tether/Window.hpp>

namespace le
{
    class DescriptorPool;
    class Material;

    class GraphicsContext
    {
    public:
        using ShaderStages = std::span<Shader::Stage>;

        GraphicsContext() = default;
        virtual ~GraphicsContext() = 0;
        LE_NO_COPY(GraphicsContext);

        virtual Scope<Renderer> CreateRenderer(RenderTarget& renderTarget) = 0;
        virtual Scope<RenderTarget> CreateHeadlessRenderTarget() = 0;
#ifndef LE_HEADLESS
        virtual Scope<RenderTarget> CreateWindowRenderTarget(Tether::Window& window) = 0;
#endif
        virtual Scope<Buffer> CreateSimpleBuffer(Buffer::Usage usage, size_t size, bool createMapped) = 0;
        virtual Scope<Buffer> CreateSmartBuffer(Buffer::Usage usage) = 0;
        virtual Scope<Buffer> CreatePerFrameBuffer(Buffer::Usage usage, size_t size) = 0;
        virtual Scope<CommandBuffer> CreateCommandBuffer(bool transfer) = 0;
        virtual Scope<DescriptorSetLayout> CreateDescriptorSetLayout(std::span<DescriptorSetLayout::Binding> bindings) = 0;
        virtual Scope<DynamicUniforms> CreateDynamicUniforms(
            std::span<DynamicUniforms::DescriptorInfo> infos) = 0;
        virtual Scope<Pipeline> CreatePipeline(std::span<Shader::Stage> stages) = 0;
        virtual Scope<Image> CreateImage(const Image::Info& info) = 0;
        virtual Scope<Sampler> CreateSampler(const Sampler::Info& info) = 0;

        virtual void RegisterShaders(ShaderManager& shaderManager) = 0;

        static Scope<GraphicsContext> Create(GraphicsAPI api,
            std::string_view applicationName);
    };
}
