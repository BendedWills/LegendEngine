module le.graphics.shader_manager;

namespace le
{
    ShaderManager::~ShaderManager()
    {}

    Shader* ShaderManager::GetByID(const std::string_view shaderID) const
    {
        if (!m_Shaders.contains(shaderID.data()))
            throw std::invalid_argument("Invalid shader ID. Was it registered?");

        return m_Shaders.at(shaderID.data());
    }

    void ShaderManager::RegisterShader(const std::string_view id, Shader& shader)
    {
        m_Shaders[id.data()] = &shader;
    }
}
