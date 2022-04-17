#ifndef _LEGENDENGINE_SHADER_HPP
#define _LEGENDENGINE_SHADER_HPP

#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Common/NativeHolder.hpp>
#include <LegendEngine/Resources/IResource.hpp>
#include <LegendEngine/Graphics/IRenderer.hpp>

#include <stdint.h>

namespace LegendEngine::Resources
{
    struct ShaderStage
    {
		uint8_t* compiledShaderCode = nullptr;
		bool compiled = false;

        std::string shaderCode;
        ShaderType type;
    };

	class Shader;
	class IShaderNative
	{
	public:
		LEGENDENGINE_NO_COPY(IShaderNative);

        IShaderNative(Shader* pShader)
			:
            pShader(pShader)
		{}

        virtual bool OnCreate(ShaderStage* pStages, uint64_t stageCount) 
        { return false;  }
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

        Shader(IRenderer* pRenderer, RenderingAPI type)
            :
            pRenderer(pRenderer),
            type(type)
        {}
        
        /**
         * @brief Initializes the shader.
         * 
         * @param pStages An array of shader stages.
         * @param stageCount The amount of stages in pStages.
         * @param uniformCount The amount of uniforms in pDescs.
         * 
         * @returns True if the vertex buffer was successfully initialized;
         *  otherwise, false.
         */
        bool Init(ShaderStage* pStages, uint64_t stageCount);

        RenderingAPI GetType();
    protected:
        IRenderer* pRenderer = nullptr;
        RenderingAPI type;
    private:
        void OnResourceDispose();
    };
}

#endif //_LEGENDENGINE_SHADER_HPP