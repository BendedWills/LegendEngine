#ifndef _LEGENDENGINE_SHADER_HPP
#define _LEGENDENGINE_SHADER_HPP

#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Common/Defs.hpp>
#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Graphics/IRenderer.hpp>
#include <stdint.h>

namespace LegendEngine
{
    struct UniformDesc
    {
        uint64_t location;
        std::string name;
    };

    struct ShaderStage
    {
        std::string shaderCode;
        uint8_t* compiledShaderCode = nullptr;
        bool compiled = false;
        ShaderType type;
    };

    class Shader : public IDisposable
    {
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
         * @param pDescs An array of uniform descs.
         * @param uniformCount The amount of uniforms in pDescs.
         * 
         * @returns True if the vertex buffer was successfully initialized;
         *  otherwise, false.
         */
        bool Init(ShaderStage* pStages, uint64_t stageCount, UniformDesc* pDescs,
            uint64_t uniformCount);

        RenderingAPI GetType();
    protected:
        virtual bool OnShaderCreate(ShaderStage* pStages, uint64_t stageCount, 
            UniformDesc* pDescs, uint64_t uniformCount) { return false; }
        virtual void OnShaderDispose() {}

        IRenderer* pRenderer = nullptr;
        RenderingAPI type;
    private:
        void OnDispose();
    };
}

#endif //_LEGENDENGINE_SHADER_HPP