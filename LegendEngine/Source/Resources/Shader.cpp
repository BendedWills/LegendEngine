#include <LegendEngine/Application.hpp>
#include <LegendEngine/Resources/Shader.hpp>

namespace le
{
    Scope<Shader> Shader::Create(const std::span<Stage> stages)
    {
        LE_DEBUG("Shader created");
        return Application::Get().GetGraphicsContext().CreateShader(stages);
    }
}