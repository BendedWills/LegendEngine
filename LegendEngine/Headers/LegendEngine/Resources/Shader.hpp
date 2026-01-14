#ifndef _LEGENDENGINE_RESOURCES_SHADER_HPP
#define _LEGENDENGINE_RESOURCES_SHADER_HPP

#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Common/NativeHolder.hpp>
#include <LegendEngine/Resources/IResource.hpp>

#include <stdint.h>
#include <vector>

namespace LegendEngine::Resources
{
    struct ShaderStage
    {
    	ShaderType type;
		const uint8_t* compiledShaderCode = nullptr;
		size_t compiledShaderSize = 0;
    };

	class Shader;
	class IShaderNative
	{
	public:
		virtual ~IShaderNative() = default;

		LEGENDENGINE_NO_COPY(IShaderNative);

        IShaderNative(Shader* pShader, std::span<Resources::ShaderStage>)
			:
            pShader(pShader)
		{}

		virtual void OnDispose() {}
	private:
		Shader* pShader = nullptr;
	};

    class Shader : 
        public NativeHolder<IShaderNative>,
        public IResource
    {
        friend IShaderNative;
    public:
        LEGENDENGINE_DISPOSE_ON_DESTRUCT(Shader);
        LEGENDENGINE_NO_COPY(Shader);

    	Shader() = default;
    private:
        void OnResourceDispose();
    };
}

#endif //_LEGENDENGINE_RESOURCES_SHADER_HPP