/*
 * Shader modules in Vulkan are literally just shaders.
 */

#ifndef _LEGENDENGINE_VULKAN_SHADERMODULE_HPP
#define _LEGENDENGINE_VULKAN_SHADERMODULE_HPP

#include <LegendEngine/Common/Defs.hpp>
#ifdef VULKAN_API

#include <LegendEngine/Common/IDisposable.hpp>
#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Graphics/Vulkan/Device.hpp>

#include <vulkan/vulkan.h>

#include <vector>

#ifdef SHADER_COMPILATION
#include <glslang/SPIRV/GlslangToSpv.h>
#endif

namespace LegendEngine::Vulkan
{
    class ShaderModule : public IDisposable
    {
    public:
        ShaderModule() {}
        LEGENDENGINE_NO_COPY(ShaderModule);

        bool Init(Device* pDevice, ShaderType type);
    
    #ifdef SHADER_COMPILATION
        /**
         * @brief Compiles a shader from GLSL and creates the shader.
         * 
         * @param glsl The GLSL shader code.
         * @param type The type of shader.
         * @param infoLog A pointer to a std::string to append info about the
         *  shader compilation
         * @param debugLog Same as info log but for debug messages.
         * 
         * @returns True if the shader compiled successfully. Otherwise, false.
         */
        bool FromGLSL(const char* glsl,
            std::string* infoLog = nullptr, std::string* debugLog = nullptr);
    #endif
        /**
         * @brief Creates the shader module from SPIR-V data.
         * 
         * @param spv The SPIR-V bytecode.
         * @param spvSize The size of the SPIR-V data.
         * 
         * @returns True if creation was successful. Otherwise, false.
         */
        bool FromSpirV(uint32_t* spv, uint64_t spvSize); 

    #ifdef SHADER_COMPILATION
        /**
         * @brief Compiles a shader from GLSL. Requires GLSLANG to be initialized.
         * 
         * @param glsl The GLSL shader code.
         * @param type The type of shader.
         * 
         * @returns An std::vector of SPIR-V shader data.
         */
        bool CompileGLSL(
            const char* glsl, 
            std::vector<uint32_t>& spv, 
            std::string& infoLog,
            std::string& debugLog
        );
    #endif

        ShaderType GetType();
        VkShaderModule Get();
    protected:
        void OnDispose();
    private:
    #ifdef SHADER_COMPILATION
        void InitResources(TBuiltInResource& resources);
    #endif

        Device* pDevice = nullptr;

        ShaderType type;
        VkShaderModule shaderModule;
        bool shaderCreated = false;
    };
}

#endif // VULKAN_API
#endif //_LEGENDENGINE_VULKAN_SHADERMODULE_HPP
