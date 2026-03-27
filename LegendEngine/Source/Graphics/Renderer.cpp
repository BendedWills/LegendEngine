#include <LE/Application.hpp>
#include <LE/Graphics/Renderer.hpp>

#include <LE/Components/ActiveCamera.hpp>
#include <LE/Components/Mesh.hpp>
#include <LE/Components/Transform.hpp>
#include <LE/Graphics/RenderTarget.hpp>
#include <LE/IO/Logger.hpp>
#include <LE/Math/Types.hpp>
#include <LE/World/Scene.hpp>

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

        Scene* sceneWithCamera = nullptr;
        UID cameraID = 0;
        ActiveCamera currentCamera;
        currentCamera.priority = std::numeric_limits<float>::min();
        for (Scene* scene : scenes)
        {
            scene->QueryArchetypes<Camera, Transform, ActiveCamera>(
            [&](const Archetype& archetype, const size_t row, Camera&, Transform&, const ActiveCamera& active)
            {
                if (active.priority >= currentCamera.priority)
                {
                    sceneWithCamera = scene;
                    cameraID = archetype.entityIDs[row];
                    currentCamera = active;
                }
            });
        }

        if (cameraID == 0)
        {
            LE_WARN("Camera was not set. Skipping frame.");
            EndFrame();
            return;
        }

        UpdateCamera(*sceneWithCamera, cameraID);

        for (Scene* pScene : scenes)
            if (pScene)
                RenderScene(*pScene);

        EndFrame();
    }

    void Renderer::RenderScene(Scene& scene)
    {
        ResourceManager& manager = Application::Get().GetResourceManager();

        Resource::ID<Material> lastMaterial = 0;
        scene.QueryComponents<Mesh, Transform>(
        [&](const Mesh& mesh, const Transform& transform)
        {
            if (!mesh.enabled)
                return;

            Ref<MeshData> resource = manager.GetResource<MeshData>(mesh.data);

            if (mesh.material != lastMaterial)
            {
                const Ref<Material> material = manager.GetResource<Material>(mesh.material);
                if (material->HasChanged())
                    material->UpdateUniforms();

                material->CopyUniformData();

                UseMaterial(*material);
                lastMaterial = mesh.material;
            }

            DrawMesh(mesh, transform);
        });
    }

    void Renderer::SetClearColor(const Vector4f& color)
    {
        m_ClearColor = color;
    }

    RenderTarget& Renderer::GetRenderTarget() const
    {
        return m_RenderTarget;
    }

    void Renderer::UpdateCamera(Scene& scene, const UID cameraID)
    {
        bool updated = false;

        scene.QueryEntityComponents<Camera, Transform>(cameraID, [&](Camera& camera, Transform& transform)
        {
            if (camera.IsCameraDirty())
            {
                camera.CalculateProjectionMatrix();
                updated = true;
            }

            if (transform.dirty)
            {
                camera.CalculateViewMatrix(transform);
                transform.dirty = true;
                updated = true;
            }
        });

        if (updated)
            UpdateCameraUniforms(scene.GetComponentData<Camera>(cameraID));
    }
}
