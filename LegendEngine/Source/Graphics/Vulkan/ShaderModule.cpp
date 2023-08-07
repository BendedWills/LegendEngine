#include <LegendEngine/Graphics/Vulkan/ShaderModule.hpp>

namespace LegendEngine::Vulkan
{
	ShaderModule::ShaderModule(TetherVulkan::GraphicsContext& context, ShaderType type)
		:
		m_Device(context.GetDevice()),
		type(type)
	{}

#ifdef SHADER_COMPILATION
	bool ShaderModule::FromGLSL(const char* glsl,
		std::string* infoLog, std::string* debugLog)
	{
		std::vector<uint32_t> spirv;

		std::string info;
		std::string debug;
		if (!CompileGLSL(glsl, type, spirv, info, debug)
			|| spirv.size() == 0)
		{
			if (infoLog)
				infoLog->append(info);
			if (debugLog)
				debugLog->append(debug);

			return false;
		}

		if (infoLog)
			infoLog->append(info);
		if (debugLog)
			debugLog->append(debug);

		return FromSpirV(spirv.data(), spirv.size());
	}
#endif

	void ShaderModule::FromSpirV(uint32_t* spv, uint64_t spvSize)
	{
		if (shaderModule)
			return;

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = spvSize;
		createInfo.pCode = spv;

		if (vkCreateShaderModule(m_Device, &createInfo, nullptr,
			&shaderModule) != VK_SUCCESS)
			throw std::runtime_error("Failed to create shader module");
	}

#ifdef SHADER_COMPILATION
	bool ShaderModule::CompileGLSL(
		const char* glsl,
		std::vector<uint32_t>& spv,
		std::string& infoLog,
		std::string& debugLog
	)
	{
		EShLanguage stage;
		switch (type)
		{
		case ShaderType::VERTEX: stage = EShLangVertex; break;
		case ShaderType::GEOM: stage = EShLangGeometry; break;
		case ShaderType::COMPUTE: stage = EShLangCompute; break;
		case ShaderType::FRAG: stage = EShLangFragment; break;
		}

		glslang::TShader shader(stage);
		glslang::TProgram program;

		const char* shaderStrings[1];
		shaderStrings[0] = glsl;

		shader.setStrings(shaderStrings, 1);

		TBuiltInResource resources = {};
		InitResources(resources);

		EShMessages messages = (EShMessages)(EShMsgSpvRules | EShMsgVulkanRules);

		if (!shader.parse(&resources, 100, false, messages))
		{
			infoLog.append(shader.getInfoLog());
			debugLog.append(shader.getInfoDebugLog());
			return false;
		}

		program.addShader(&shader);

		if (!program.link(messages))
		{
			infoLog.append(shader.getInfoLog());
			debugLog.append(shader.getInfoDebugLog());
			return false;
		}

		infoLog.append(shader.getInfoLog());
		debugLog.append(shader.getInfoDebugLog());

		glslang::GlslangToSpv(*program.getIntermediate(stage), spv);
		return true;
	}
#endif

	LegendEngine::ShaderType ShaderModule::GetType()
	{
		return type;
	}

	VkShaderModule ShaderModule::Get()
	{
		return shaderModule;
	}

	void ShaderModule::OnDispose()
	{
		if (!shaderCreated)
			return;

		shaderCreated = false;
		vkDestroyShaderModule(m_Device, shaderModule, nullptr);
	}

#ifdef SHADER_COMPILATION
	void ShaderModule::InitResources(TBuiltInResource& resources)
	{
		resources.maxLights = 32;
		resources.maxClipPlanes = 6;
		resources.maxTextureUnits = 32;
		resources.maxTextureCoords = 32;
		resources.maxVertexAttribs = 64;
		resources.maxVertexUniformComponents = 4096;
		resources.maxVaryingFloats = 64;
		resources.maxVertexTextureImageUnits = 32;
		resources.maxCombinedTextureImageUnits = 80;
		resources.maxTextureImageUnits = 32;
		resources.maxFragmentUniformComponents = 4096;
		resources.maxDrawBuffers = 32;
		resources.maxVertexUniformVectors = 128;
		resources.maxVaryingVectors = 8;
		resources.maxFragmentUniformVectors = 16;
		resources.maxVertexOutputVectors = 16;
		resources.maxFragmentInputVectors = 15;
		resources.minProgramTexelOffset = -8;
		resources.maxProgramTexelOffset = 7;
		resources.maxClipDistances = 8;
		resources.maxComputeWorkGroupCountX = 65535;
		resources.maxComputeWorkGroupCountY = 65535;
		resources.maxComputeWorkGroupCountZ = 65535;
		resources.maxComputeWorkGroupSizeX = 1024;
		resources.maxComputeWorkGroupSizeY = 1024;
		resources.maxComputeWorkGroupSizeZ = 64;
		resources.maxComputeUniformComponents = 1024;
		resources.maxComputeTextureImageUnits = 16;
		resources.maxComputeImageUniforms = 8;
		resources.maxComputeAtomicCounters = 8;
		resources.maxComputeAtomicCounterBuffers = 1;
		resources.maxVaryingComponents = 60;
		resources.maxVertexOutputComponents = 64;
		resources.maxGeometryInputComponents = 64;
		resources.maxGeometryOutputComponents = 128;
		resources.maxFragmentInputComponents = 128;
		resources.maxImageUnits = 8;
		resources.maxCombinedImageUnitsAndFragmentOutputs = 8;
		resources.maxCombinedShaderOutputResources = 8;
		resources.maxImageSamples = 0;
		resources.maxVertexImageUniforms = 0;
		resources.maxTessControlImageUniforms = 0;
		resources.maxTessEvaluationImageUniforms = 0;
		resources.maxGeometryImageUniforms = 0;
		resources.maxFragmentImageUniforms = 8;
		resources.maxCombinedImageUniforms = 8;
		resources.maxGeometryTextureImageUnits = 16;
		resources.maxGeometryOutputVertices = 256;
		resources.maxGeometryTotalOutputComponents = 1024;
		resources.maxGeometryUniformComponents = 1024;
		resources.maxGeometryVaryingComponents = 64;
		resources.maxTessControlInputComponents = 128;
		resources.maxTessControlOutputComponents = 128;
		resources.maxTessControlTextureImageUnits = 16;
		resources.maxTessControlUniformComponents = 1024;
		resources.maxTessControlTotalOutputComponents = 4096;
		resources.maxTessEvaluationInputComponents = 128;
		resources.maxTessEvaluationOutputComponents = 128;
		resources.maxTessEvaluationTextureImageUnits = 16;
		resources.maxTessEvaluationUniformComponents = 1024;
		resources.maxTessPatchComponents = 120;
		resources.maxPatchVertices = 32;
		resources.maxTessGenLevel = 64;
		resources.maxViewports = 16;
		resources.maxVertexAtomicCounters = 0;
		resources.maxTessControlAtomicCounters = 0;
		resources.maxTessEvaluationAtomicCounters = 0;
		resources.maxGeometryAtomicCounters = 0;
		resources.maxFragmentAtomicCounters = 8;
		resources.maxCombinedAtomicCounters = 8;
		resources.maxAtomicCounterBindings = 1;
		resources.maxVertexAtomicCounterBuffers = 0;
		resources.maxTessControlAtomicCounterBuffers = 0;
		resources.maxTessEvaluationAtomicCounterBuffers = 0;
		resources.maxGeometryAtomicCounterBuffers = 0;
		resources.maxFragmentAtomicCounterBuffers = 1;
		resources.maxCombinedAtomicCounterBuffers = 1;
		resources.maxAtomicCounterBufferSize = 16384;
		resources.maxTransformFeedbackBuffers = 4;
		resources.maxTransformFeedbackInterleavedComponents = 64;
		resources.maxCullDistances = 8;
		resources.maxCombinedClipAndCullDistances = 8;
		resources.maxSamples = 4;
		resources.maxMeshOutputVerticesNV = 256;
		resources.maxMeshOutputPrimitivesNV = 512;
		resources.maxMeshWorkGroupSizeX_NV = 32;
		resources.maxMeshWorkGroupSizeY_NV = 1;
		resources.maxMeshWorkGroupSizeZ_NV = 1;
		resources.maxTaskWorkGroupSizeX_NV = 32;
		resources.maxTaskWorkGroupSizeY_NV = 1;
		resources.maxTaskWorkGroupSizeZ_NV = 1;
		resources.maxMeshViewCountNV = 4;
		resources.limits.nonInductiveForLoops = 1;
		resources.limits.whileLoops = 1;
		resources.limits.doWhileLoops = 1;
		resources.limits.generalUniformIndexing = 1;
		resources.limits.generalAttributeMatrixVectorIndexing = 1;
		resources.limits.generalVaryingIndexing = 1;
		resources.limits.generalSamplerIndexing = 1;
		resources.limits.generalVariableIndexing = 1;
		resources.limits.generalConstantMatrixVectorIndexing = 1;
	}
#endif
}