#pragma once

#include <LegendEngine/Scene.hpp>
#include <LegendEngine/Components/MeshComponent.hpp>
#include <LegendEngine/Graphics/RenderTarget.hpp>
#include <LegendEngine/Objects/Camera.hpp>
#include <LegendEngine/Resources/Material.hpp>
#include <LegendEngine/Resources/Shader.hpp>

namespace le
{
    class Application;
}

namespace le
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
        virtual void UseMaterial(Material* pMaterial) = 0;
        virtual void DrawMesh(const MeshComponent& mesh) = 0;
        virtual void EndFrame() = 0;

        virtual void UpdateCameraUniforms(const Camera& camera) = 0;

        RenderTarget& m_RenderTarget;
    private:
        void RenderScene(Scene& scene);
        void UpdateCamera(Camera* pCamera);

        Scene* m_Scene = nullptr;
    };
}