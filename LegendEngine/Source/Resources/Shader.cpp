#include <LegendEngine/Application.hpp>
#include <LegendEngine/Resources/Shader.hpp>

namespace le
{
    void Shader::SetCullMode(const CullMode cullMode)
    {
        m_CullMode = cullMode;
    }

    Shader::CullMode Shader::GetCullMode() const
    {
        return m_CullMode;
    }

    Scope<Shader> Shader::Create(const std::span<Stage> stages)
    {
        LE_DEBUG("Shader created");
        return Application::Get().GetGraphicsContext().CreateShader(stages);
    }
}