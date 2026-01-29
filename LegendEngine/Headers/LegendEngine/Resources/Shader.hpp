#pragma once

#include <LegendEngine/Resources/Resource.hpp>
#include <LegendEngine/Common/Types.hpp>

#include <memory>
#include <span>

namespace LegendEngine::Resources
{
    class Shader : public Resource
    {
    public:
        struct Stage
        {
            ShaderType type;
            const uint8_t* compiledShaderCode = nullptr;
            size_t compiledShaderSize = 0;
        };

        static Scope<Shader> Create(std::span<Stage> stages);
    };
}