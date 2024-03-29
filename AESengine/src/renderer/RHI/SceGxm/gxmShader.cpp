#include "gxmShader.hpp"
#include "gxmElements.hpp"
#include "gxmDevice.hpp"
#include "core/utility.hpp"
#include <vector>

using namespace aes;

GxmShader::GxmShader(GxmShader&& rhs) noexcept : gxpShader(rhs.gxpShader), id(rhs.id)
{
	AES_PROFILE_FUNCTION();

	rhs.gxpShader = nullptr;
}

GxmShader& GxmShader::operator=(GxmShader&& rhs) noexcept
{
	AES_PROFILE_FUNCTION();

	gxpShader = rhs.gxpShader;
	id = rhs.id;
	rhs.gxpShader = nullptr;
	return *this;
}

GxmShader::~GxmShader()
{
	AES_PROFILE_FUNCTION();
	sceGxmShaderPatcherUnregisterProgram(gxmShaderPatcher, id);
}

static size_t getProgramParameterTypeSize(SceGxmParameterType type)
{
	switch (type)
	{
		case SCE_GXM_PARAMETER_TYPE_U32:
		case SCE_GXM_PARAMETER_TYPE_F32:
			return 4;
		case SCE_GXM_PARAMETER_TYPE_F16:
		case SCE_GXM_PARAMETER_TYPE_U16:
		case SCE_GXM_PARAMETER_TYPE_S16:
		case SCE_GXM_PARAMETER_TYPE_C10:
			return 2;
		case SCE_GXM_PARAMETER_TYPE_U8:
		case SCE_GXM_PARAMETER_TYPE_S8:
			return 1;
		case SCE_GXM_PARAMETER_TYPE_AGGREGATE:
			AES_ASSERT(false);
	}

	AES_UNREACHABLE();
}

static size_t getProgramParameterSize(SceGxmProgramParameter const* param)
{
	return sceGxmProgramParameterGetArraySize(param) * getProgramParameterTypeSize(sceGxmProgramParameterGetType(param));
}

// Review
#if 0
std::vector<UniformBufferReflectionInfo> GxmShader::getUniformBufferInfos() const
{
	AES_PROFILE_FUNCTION();

	uint32_t const paramsCount = sceGxmProgramGetParameterCount(gxpShader);
	std::vector<UniformBufferReflectionInfo> uniformBuffersInfos;
	for (uint32_t i = 0; i < paramsCount; i++)
	{
		SceGxmProgramParameter const* param = sceGxmProgramGetParameter(gxpShader, i);
		AES_ASSERT(param != nullptr); // if i is in bounds param should not be null

		SceGxmParameterCategory parameterCategory = sceGxmProgramParameterGetCategory(param);
		if (parameterCategory == SCE_GXM_PARAMETER_CATEGORY_UNIFORM_BUFFER)
		{
			// uniformBuffersInfos.push_back({
			// 	.name = fmt::format("BUFFER{}", sceGxmProgramParameterGetResourceIndex(param)),
			// 	.index = sceGxmProgramParameterGetResourceIndex(param),
			// 	.size = 0 });
		}
		else if (parameterCategory == SCE_GXM_PARAMETER_CATEGORY_UNIFORM)
		{
			std::string const bufferName = split(sceGxmProgramParameterGetName(param), '.')[0];
			auto const it = std::find_if(uniformBuffersInfos.begin(), uniformBuffersInfos.end(), [&bufferName](auto const& a) { return a.name == bufferName; });
			if (it != uniformBuffersInfos.end())
			{
				// update infos about the existing buffer
				// TODO deduce the buffer size according his members and ubo alignement
				// it->size += getProgramParameterSize(param); // unsupported for now
			}
			else
			{
				// New buffer read add it
				uniformBuffersInfos.push_back({
						.name = std::move(bufferName), 
						.index = sceGxmProgramParameterGetContainerIndex(param),
						.size = getProgramParameterSize(param) });
			}
		}
	}
	return uniformBuffersInfos;
}
#endif 

static auto getFormatComponents(RHIFormat format)
{
	AES_PROFILE_FUNCTION();
	struct FormatComponent
	{
		SceGxmAttributeFormat attribFormat;
		uint32_t numComponents;
	};

	switch(format)
	{
		case RHIFormat::R8G8B8A8_Uint:
			return FormatComponent{ SCE_GXM_ATTRIBUTE_FORMAT_U8N, 4 };
		case RHIFormat::R16G16_Float:
			return FormatComponent{ SCE_GXM_ATTRIBUTE_FORMAT_F16, 2 };
		case RHIFormat::R32G32_Float:
			return FormatComponent{ SCE_GXM_ATTRIBUTE_FORMAT_F32, 2 };
		case RHIFormat::R32G32B32_Float:
			return FormatComponent{ SCE_GXM_ATTRIBUTE_FORMAT_F32, 3 };
		case RHIFormat::R16G16B16A16_Float:
			return FormatComponent{ SCE_GXM_ATTRIBUTE_FORMAT_F16, 4 };
		case RHIFormat::R32G32B32A32_Float:
			return FormatComponent{ SCE_GXM_ATTRIBUTE_FORMAT_F32, 4 };
	}
	AES_UNREACHABLE();
}

GxmVertexShader::GxmVertexShader(GxmVertexShader&& rhs) noexcept
	: GxmShader(std::move(rhs)), vertexShader(rhs.vertexShader)
{
	AES_PROFILE_FUNCTION();
	rhs.vertexShader = nullptr;
}

GxmVertexShader& GxmVertexShader::operator=(GxmVertexShader&& rhs) noexcept
{
	AES_PROFILE_FUNCTION();
	GxmShader::operator=(std::move(rhs));
	vertexShader = rhs.vertexShader;
	rhs.vertexShader = nullptr;
	return *this;
}

GxmVertexShader::~GxmVertexShader()
{
	AES_PROFILE_FUNCTION();
	if (vertexShader != nullptr)
	{
		GxmShader::~GxmShader();
		sceGxmShaderPatcherReleaseVertexProgram(gxmShaderPatcher, vertexShader);
		vertexShader = nullptr;
	}
}

Result<void> GxmVertexShader::init(VertexShaderDescription const& desc)
{
	AES_PROFILE_FUNCTION();
	AES_ASSERTF(std::holds_alternative<uint8_t const*>(desc.source), "runtime compiled shaders aren't supported on vita platform ! {}", std::holds_alternative<uint8_t const*>(desc.source));
	
	gxpShader = reinterpret_cast<SceGxmProgram const*>(std::get<uint8_t const*>(desc.source));
	AES_ASSERT(gxpShader != nullptr);
	AES_ASSERT(sceGxmProgramCheck(gxpShader) == SCE_OK);
	AES_ASSERT(sceGxmProgramGetType(gxpShader) == SCE_GXM_VERTEX_PROGRAM);

	auto err = sceGxmShaderPatcherRegisterProgram(gxmShaderPatcher, gxpShader, &id);
	if (err != SCE_OK)
	{
		AES_LOG_ERROR("sceGxmShaderPatcherRegisterProgram {}", err);
		return { AESError::ShaderCreationFailed };
	}

	std::vector<SceGxmVertexAttribute> verticesAttributes(desc.verticesLayout.size());
	for (int i = 0; auto const& layout : desc.verticesLayout)
	{
		verticesAttributes[i].streamIndex = 0;
		verticesAttributes[i].offset = layout.offset;
		auto const formatComponents = getFormatComponents(layout.format);
		verticesAttributes[i].format = formatComponents.attribFormat;
		verticesAttributes[i].componentCount = formatComponents.numComponents;
		SceGxmProgramParameter const* param = sceGxmProgramFindParameterByName(gxpShader, layout.parameterName);
		AES_ASSERTF(param != nullptr, "param {} not found", layout.parameterName);
		verticesAttributes[i].regIndex = sceGxmProgramParameterGetResourceIndex(param);
		i++;
	}
	
	SceGxmVertexStream vertexStream;
	vertexStream.stride = desc.verticesStride;
	vertexStream.indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT; // @Review
	// vertexStream.indexSource = SCE_GXM_INDEX_SOURCE_INDEX_32BIT; // @Review

	err = sceGxmShaderPatcherCreateVertexProgram(gxmShaderPatcher, id, verticesAttributes.data(), verticesAttributes.size(), &vertexStream, 1, &vertexShader);
	if (err != SCE_OK)
	{
		AES_LOG_ERROR("sceGxmShaderPatcherCreateVertexProgram {}", err);
		return { AESError::ShaderCreationFailed };
	}

	return {};
}

SceGxmVertexProgram* GxmVertexShader::getHandle()
{
	return vertexShader;
}

GxmFragmentShader::GxmFragmentShader(GxmFragmentShader&& rhs) noexcept : GxmShader(std::move(rhs)), fragmentShader(rhs.fragmentShader)
{
	AES_PROFILE_FUNCTION();
	rhs.fragmentShader = nullptr;
}

GxmFragmentShader& GxmFragmentShader::operator=(GxmFragmentShader&& rhs) noexcept
{
	AES_PROFILE_FUNCTION();
	GxmShader::operator=(std::move(rhs));
	fragmentShader = rhs.fragmentShader;
	rhs.fragmentShader = nullptr;
	return *this;
}

GxmFragmentShader::~GxmFragmentShader()
{
	AES_PROFILE_FUNCTION();
	if (fragmentShader != nullptr)
	{
		GxmShader::~GxmShader();
		sceGxmShaderPatcherReleaseFragmentProgram(gxmShaderPatcher, fragmentShader);
		fragmentShader = nullptr;
	}
}

Result<void> GxmFragmentShader::init(FragmentShaderDescription const& desc)
{
	AES_PROFILE_FUNCTION();
	AES_ASSERTF(std::holds_alternative<uint8_t const*>(desc.source), "runtime compiled shaders aren't supported on vita platform ! {}", std::holds_alternative<uint8_t const*>(desc.source));
	
	gxpShader = reinterpret_cast<SceGxmProgram const*>(std::get<uint8_t const*>(desc.source));
	AES_ASSERT(gxpShader);
	AES_ASSERT(sceGxmProgramCheck(gxpShader) == SCE_OK);
	AES_ASSERT(sceGxmProgramGetType(gxpShader) == SCE_GXM_FRAGMENT_PROGRAM);
	
	auto err = sceGxmShaderPatcherRegisterProgram(gxmShaderPatcher, gxpShader, &id);
	if (err != SCE_OK)
	{
		AES_LOG_ERROR("failed to register fragment shader");
		return { AESError::ShaderCreationFailed };
	}
	
	SceGxmBlendInfo* pblendInfo = nullptr;
	SceGxmBlendInfo blendInfo{};
	if (desc.blendInfo)
	{
		// @TODO assert SCE_GXM_OUTPUT_REGISTER_FORMAT to be either UCHAR4 or HALF4
		blendInfo.colorFunc = rhiBlendOpToApi(desc.blendInfo->colorOp);
		blendInfo.alphaFunc = rhiBlendOpToApi(desc.blendInfo->alphaOp);
		blendInfo.colorSrc  = rhiBlendFactorToApi(desc.blendInfo->colorSrc);
		blendInfo.colorDst  = rhiBlendFactorToApi(desc.blendInfo->colorDst);
		blendInfo.alphaSrc  = rhiBlendFactorToApi(desc.blendInfo->alphaSrc);
		blendInfo.alphaDst  = rhiBlendFactorToApi(desc.blendInfo->alphaDst);
		blendInfo.colorMask = rhiColorMaskToApi(desc.blendInfo->colorMask);
		pblendInfo = &blendInfo;
	}

	err = sceGxmShaderPatcherCreateFragmentProgram(gxmShaderPatcher, id, SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4, 
		rhiMultisampleModeToApi(desc.multisampleMode), pblendInfo, (SceGxmProgram const*)NULL/*REVIEW why this glitch*/, &fragmentShader);
	if (err != SCE_OK)
	{
		AES_LOG_ERROR("failed to create fragment shader err: {}", err);
		return { AESError::ShaderCreationFailed };
	}

	return {};
}

SceGxmFragmentProgram* GxmFragmentShader::getHandle()
{
	return fragmentShader;
}


