#pragma once

#include <LegendEngine/Common/Defs.hpp>

#include <LegendEngine/Common/Types.hpp>
#include <LegendEngine/Graphics/Vulkan/GraphicsContextVk.hpp>

#include <vulkan/vulkan.h>

#include <vector>

#ifdef SHADER_COMPILATION
#include <glslang/SPIRV/GlslangToSpv.h>
#endif

namespace LegendEngine::Vulkan
{
    class ShaderModule
    {
    public:
        ShaderModule(TetherVulkan::GraphicsContext& context, ShaderType type);
        LEGENDENGINE_NO_COPY(ShaderModule);

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
        void FromSpirV(uint32_t* spv, uint64_t spvSize); 

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

        VkDevice m_Device = nullptr;

        ShaderType type;
        VkShaderModule shaderModule;
        bool shaderCreated = false;
    };
}
