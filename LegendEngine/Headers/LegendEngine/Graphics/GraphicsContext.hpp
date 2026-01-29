#pragma once

#include <memory>
#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/Logger.hpp>
#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Graphics/Renderer.hpp>
#include <LegendEngine/Graphics/VertexBuffer.hpp>
#include <LegendEngine/Resources/Material.hpp>
#include <LegendEngine/Resources/Shader.hpp>

namespace LegendEngine::Graphics
{
    class GraphicsContext
    {
    public:
        using ShaderStages = std::span<Resources::Shader::Stage>;

        GraphicsContext() = default;
        virtual ~GraphicsContext() = 0;
        LEGENDENGINE_NO_COPY(GraphicsContext);

        virtual Scope<Renderer> CreateRenderer(Application& app) = 0;

        virtual Scope<VertexBuffer> CreateVertexBuffer(
            std::span<VertexTypes::Vertex3> vertices,
            std::span<uint32_t> indices) = 0;
        virtual Scope<Resources::Material> CreateMaterial() = 0;
        virtual Scope<Resources::Texture2D> CreateTexture2D(IO::TextureLoader& loader) = 0;

        static Scope<GraphicsContext> Create(GraphicsAPI api,
            std::string_view applicationName, bool debug, Logger& logger);
    };
}
