#pragma once

#include <LE/Common/Defs.hpp>
#include <LE/Graphics/RenderTarget.hpp>
#include <LE/Math/Types.hpp>

namespace le
{
    class Application;
    class GraphicsContext;
    class Renderer
    {
    public:
        explicit Renderer(RenderTarget& renderTarget);
        virtual ~Renderer() = 0;
        LE_NO_COPY(Renderer);

        void RenderFrame(std::span<Scene*> scenes);

        void SetClearColor(const Vector4f& color);
        virtual void SetVSyncEnabled(bool vsync) {}

        virtual void NotifyWindowResized() {}

        [[nodiscard]] RenderTarget& GetRenderTarget() const;
    protected:
        virtual bool StartFrame() = 0;
        virtual void UseMaterial(Material* pMaterial) = 0;
        virtual void DrawMesh(const Mesh& mesh) = 0;
        virtual void EndFrame() = 0;

        virtual void UpdateCameraUniforms(const Camera& camera) = 0;

        RenderTarget& m_RenderTarget;
        Vector4f m_ClearColor = Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
    private:
        void RenderScene(Scene& scene);
        void UpdateCamera(Camera* pCamera);

        Scene* m_Scene = nullptr;
    };
}
