#pragma once

#include <LegendEngine/Resources/Shader.hpp>
#include <unordered_map>

namespace le
{
    class ShaderManager
    {
    public:
        ShaderManager() = default;
        virtual ~ShaderManager() = 0;
        LEGENDENGINE_NO_COPY(ShaderManager);

        Shader* GetByID(std::string_view shaderID) const;
    protected:
        // The shader object MUST exist for the life of the base class
        void RegisterShader(std::string_view id, Shader& shader);
    private:
        std::unordered_map<std::string, Shader*> m_Shaders;
    };
}