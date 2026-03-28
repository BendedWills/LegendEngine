#pragma once

#include <LE/Graphics/ShaderManager.hpp>

namespace le
{
    class GraphicsResources final
    {
    public:
        [[nodiscard]] ShaderManager& GetShaderManager();


    private:
        ShaderManager m_shaderManager;
    };
}
