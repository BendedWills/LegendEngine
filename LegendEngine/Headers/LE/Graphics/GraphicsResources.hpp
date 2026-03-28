#pragma once

#include <LE/Graphics/GraphicsContext.hpp>
#include <LE/Graphics/ShaderManager.hpp>
#include <LE/Graphics/API/DescriptorSetLayout.hpp>

namespace le
{
    class GraphicsResources final
    {
    public:
        explicit GraphicsResources(GraphicsContext& context);

        [[nodiscard]] ShaderManager& GetShaderManager();

        DescriptorSetLayout& GetCameraLayout() const;
        DescriptorSetLayout& GetMaterialLayout() const;
        DescriptorSetLayout& GetSceneLayout() const;
        std::span<DescriptorSetLayout*> GetLayouts();
    private:
        void CreateCameraDescriptorSetLayout();
        void CreateSceneDescriptorSetLayout();
        void CreateMaterialDescriptorSetLayout();

        GraphicsContext& m_context;

        ShaderManager m_shaderManager;

        Scope<DescriptorSetLayout> m_cameraLayout = nullptr;
        Scope<DescriptorSetLayout> m_sceneLayout = nullptr;
        Scope<DescriptorSetLayout> m_materialLayout = nullptr;
        std::vector<DescriptorSetLayout*> m_setLayouts;
    };
}
