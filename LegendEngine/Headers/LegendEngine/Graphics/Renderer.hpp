#pragma once

#include <LegendEngine/Scene.hpp>
#include <LegendEngine/Components/MeshComponent.hpp>
#include <LegendEngine/Objects/Camera.hpp>
#include <LegendEngine/Resources/Material.hpp>
#include <LegendEngine/Resources/Shader.hpp>

namespace LegendEngine
{
    class Application;
}

namespace LegendEngine::Graphics
{
    class Renderer
    {
    public:
        explicit Renderer(Application& app);
        virtual ~Renderer();
        LEGENDENGINE_NO_COPY(Renderer);

        void RenderFrame();

        virtual Scope<Resources::Shader> CreateShader(
            std::span<Resources::Shader::Stage> stages) = 0;
        virtual Scope<Resources::Material> CreateMaterial() = 0;

        virtual void SetVSyncEnabled(bool vsync) {}
        virtual void NotifyWindowResized() {}
    protected:
        virtual bool StartFrame() = 0;
        virtual void UseMaterial(Resources::Material* pMaterial) = 0;
        virtual void DrawMesh(const Components::MeshComponent& mesh) = 0;
        virtual void EndFrame() = 0;

        virtual void UpdateCameraUniforms(const Objects::Camera& camera) = 0;

        Application& m_App;
    private:
        void RenderScene(Scene& scene);

        Scene* m_Scene = nullptr;
    };
}