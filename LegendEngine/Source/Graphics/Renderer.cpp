#include <LegendEngine/Graphics/Renderer.hpp>
#include <LegendEngine/Application.hpp>
#include <LegendEngine/Components/MeshComponent.hpp>

namespace LegendEngine::Graphics
{
    Renderer::Renderer(Application& app)
        :
        m_App(app)
    {}

    Renderer::~Renderer()
    {}

    void Renderer::RenderFrame()
    {
        if (!StartFrame())
            return;

        Objects::Camera* pCamera = m_App.GetActiveCamera();
        if (!pCamera || !pCamera->IsEnabled())
        {
            EndFrame();
            return;
        }

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

        RenderScene(m_App.GetGlobalScene());
        if (Scene* pScene = m_App.GetActiveScene(); pScene)
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

            Material* pMaterial = component.GetMaterial();
            if (pMaterial != lastMaterial)
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
}
