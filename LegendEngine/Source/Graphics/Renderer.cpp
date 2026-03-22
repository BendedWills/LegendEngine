#include <LE/Camera.hpp>
#include <LE/Scene.hpp>
#include <LE/Components/Mesh.hpp>
#include <LE/Graphics/Renderer.hpp>
#include <LE/Graphics/RenderTarget.hpp>
#include <LE/Math/Types.hpp>

namespace le
{
    Renderer::Renderer(RenderTarget& renderTarget)
        :
        m_RenderTarget(renderTarget)
    {}

    Renderer::~Renderer() = default;

    void Renderer::RenderFrame(const std::span<Scene*> scenes)
    {
        if (!StartFrame())
            return;

        Camera* pCamera = m_RenderTarget.GetCamera();
        if (!pCamera)
        {
            EndFrame();
            return;
        }

        UpdateCamera(pCamera);

        for (Scene* pScene : scenes)
            if (pScene)
                RenderScene(*pScene);

        EndFrame();
    }

    void Renderer::RenderScene(Scene& scene)
    {
        const std::type_index meshType = typeid(Mesh);
        const auto components = scene.GetObjectComponents();
        if (!components.contains(meshType))
            return;

        // TODO: this has raw pointers to the component and its object.
        // if the object is destroyed on another thread, this could cause a crash
        const std::vector<Component*>* meshComponents = &components.at(meshType);

        const Material* lastMaterial = nullptr;
        for (Component* pComponent : *meshComponents)
        {
            const Mesh& component = *static_cast<Mesh*>(pComponent);

            if (!component.GetObject().IsEnabled())
                continue;

            if (Material* pMaterial = component.GetMaterial();
                pMaterial != lastMaterial)
            {
                if (pMaterial && pMaterial->HasChanged())
                    pMaterial->UpdateUniforms();

                UseMaterial(pMaterial);
                lastMaterial = pMaterial;
            }

            DrawMesh(component);
        }
    }

    void Renderer::SetClearColor(const Vector4f& color)
    {
        m_ClearColor = color;
    }

    RenderTarget& Renderer::GetRenderTarget() const
    {
        return m_RenderTarget;
    }

    void Renderer::UpdateCamera(Camera* pCamera)
    {
        if (pCamera->IsDirty())
        {
            pCamera->CalculateViewMatrix();
            UpdateCameraUniforms(*pCamera);
        }

        if (pCamera->IsCameraDirty())
        {
            pCamera->CalculateProjectionMatrix();
            UpdateCameraUniforms(*pCamera);
        }
    }
}
