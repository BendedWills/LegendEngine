#include <LE/Application.hpp>
#include <LE/Resources/Shader.hpp>

namespace le
{
    Shader::Shader(const std::span<Stage> stages)
    {
        m_pipeline = Application::Get().GetGraphicsContext().CreatePipeline(stages);
    }

    void Shader::SetCullMode(const CullMode cullMode)
    {
        m_CullMode = cullMode;
    }

    Shader::CullMode Shader::GetCullMode() const
    {
        return m_CullMode;
    }

    Pipeline& Shader::GetPipeline() const
    {
        return *m_pipeline;
    }
}
