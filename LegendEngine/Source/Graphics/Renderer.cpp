#include <LegendEngine/Graphics/Renderer.hpp>
#include <LegendEngine/Application.hpp>
#include <LegendEngine/Components/MeshComponent.hpp>

namespace LegendEngine::Graphics
{
    Renderer::Renderer(RenderTarget& renderTarget)
        :
        m_RenderTarget(renderTarget)
    {}

    Renderer::~Renderer()
    {}

    void Renderer::RenderFrame(const std::span<Scene*> scenes)
    {
        if (!StartFrame())
            return;

        Objects::Camera* pCamera = m_RenderTarget.GetCamera();
        if (!pCamera || !pCamera->IsEnabled())
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
        using namespace Components;
        using namespace Resources;
        using namespace Objects;

        const std::type_index meshType = typeid(MeshComponent);
        const auto& components = scene.GetObjectComponents();
        if (!components.contains(meshType))
            return;
        const std::vector<Component*>* meshComponents = &components.at(meshType);

        const Material* lastMaterial = nullptr;
        for (Component* pComponent : *meshComponents)
        {
            const MeshComponent& component = *static_cast<MeshComponent*>(pComponent);

            if (!component.GetObject().IsEnabled())
                continue;

            if (Material* pMaterial = component.GetMaterial();
                pMaterial != lastMaterial)
            {
                if (pMaterial && pMaterial->HasChanged())
                {
                    pMaterial->UpdateMaterial();
                    pMaterial->SetChanged(false);
                }

                UseMaterial(pMaterial);
                lastMaterial = pMaterial;
            }

            DrawMesh(component);
        }
    }

    RenderTarget& Renderer::GetRenderTarget() const
    {
        return m_RenderTarget;
    }

    void Renderer::UpdateCamera(Objects::Camera* pCamera)
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
