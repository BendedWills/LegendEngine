#pragma once

#include <LegendEngine/Resources/Resource.hpp>
#include <LegendEngine/Common/Types.hpp>

#include <memory>
#include <span>

namespace le
{
    class Shader : public Resource
    {
    public:
        enum class CullMode
        {
            NONE,
            BACK,
            FRONT,
        };

        struct Stage
        {
            ShaderType type;
            const uint8_t* compiledShaderCode = nullptr;
            size_t compiledShaderSize = 0;
        };

        void SetCullMode(CullMode cullMode);
        CullMode GetCullMode() const;

        static Scope<Shader> Create(std::span<Stage> stages);
    protected:
        CullMode m_CullMode = CullMode::BACK;
    };
}