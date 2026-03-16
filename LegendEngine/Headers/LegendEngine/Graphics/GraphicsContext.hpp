#pragma once

#include <memory>
#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Graphics/Renderer.hpp>
#include <LegendEngine/Graphics/ShaderManager.hpp>
#include <LegendEngine/Graphics/VertexBuffer.hpp>
#include <LegendEngine/Resources/Material.hpp>
#include <LegendEngine/Resources/Shader.hpp>
#include <Tether/Window.hpp>

namespace le
{
    class GraphicsContext
    {
    public:
        using ShaderStages = std::span<Shader::Stage>;

        GraphicsContext() = default;
        virtual ~GraphicsContext() = 0;
        LEGENDENGINE_NO_COPY(GraphicsContext);

        virtual Scope<Renderer> CreateRenderer(RenderTarget& renderTarget) = 0;
        virtual Scope<RenderTarget> CreateHeadlessRenderTarget() = 0;
#ifndef LE_HEADLESS
        virtual Scope<RenderTarget> CreateWindowRenderTarget(Tether::Window& window) = 0;
#endif

        virtual Scope<VertexBuffer> CreateVertexBuffer(size_t initialVertexCount,
            size_t initialIndexCount, VertexBuffer::UpdateFrequency frequency) = 0;
        virtual Scope<Texture2D> CreateTexture2D(const TextureData& loader) = 0;
        virtual Scope<Shader> CreateShader(
            std::span<Shader::Stage> stages) = 0;
        virtual Scope<Material> CreateMaterial() = 0;

        [[nodiscard]] virtual const ShaderManager& GetShaderManager() const = 0;

        static Scope<GraphicsContext> Create(GraphicsAPI api,
            std::string_view applicationName);
    };
}
