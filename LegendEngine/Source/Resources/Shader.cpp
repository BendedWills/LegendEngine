#include <LegendEngine/Application.hpp>
#include <LegendEngine/Resources/Shader.hpp>

namespace LegendEngine::Resources
{
    Scope<Shader> Shader::Create(const std::span<Stage> stages)
    {
        LGENG_DEBUG_LOG("Shader created");
        return Application::Get().GetGraphicsContext().CreateShader(stages);
    }
}