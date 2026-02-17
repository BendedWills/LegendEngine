#pragma once

#include <LegendEngine/Scene.hpp>
#include <LegendEngine/Components/MeshComponent.hpp>
#include <LegendEngine/Graphics/RenderTarget.hpp>
#include <LegendEngine/Objects/Camera.hpp>
#include <LegendEngine/Resources/Material.hpp>
#include <LegendEngine/Resources/Shader.hpp>

namespace LegendEngine
{
    class Application;
}

namespace LegendEngine::Graphics
{
    class GraphicsContext;
    class Renderer
    {
    public:
        explicit Renderer(RenderTarget& renderTarget);
        virtual ~Renderer() = 0;
        LEGENDENGINE_NO_COPY(Renderer);

        void RenderFrame(std::span<Scene*> scenes);

        virtual void SetVSyncEnabled(bool vsync) {}
        virtual void NotifyWindowResized() {}

        RenderTarget& GetRenderTarget() const;
    protected:
        virtual bool StartFrame() = 0;
        virtual void UseMaterial(Resources::Material* pMaterial) = 0;
        virtual void DrawMesh(const Components::MeshComponent& mesh) = 0;
        virtual void EndFrame() = 0;

        virtual void UpdateCameraUniforms(const Objects::Camera& camera) = 0;

        RenderTarget& m_RenderTarget;
    private:
        void RenderScene(Scene& scene);
        void UpdateCamera(Objects::Camera* pCamera);

        Scene* m_Scene = nullptr;
    };
}