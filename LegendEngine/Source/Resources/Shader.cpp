#include <LegendEngine/Application.hpp>
#include <LegendEngine/Resources/Shader.hpp>

namespace LegendEngine::Resources
{
    Scope<Shader> Shader::Create(const std::span<Stage> stages)
    {
        return Application::Get().GetRenderer().CreateShader(stages);
    }
}