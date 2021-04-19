#include "gxmShader.hpp"
#include "gxmElements.hpp"
#include "gxmRenderer.hpp"
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

	sceGxmShaderPatcherUnregisterProgram(RHIRenderContext::instance().getShaderPatcher(), id);
}

std::vector<UniformBufferReflectionInfo> GxmShader::getUniformBufferInfos() const
{
	AES_PROFILE_FUNCTION();

	AES_LOG("get getUniformBufferInfos start");
	uint32_t const paramsCount = sceGxmProgramGetParameterCount(gxpShader);
	std::vector<UniformBufferReflectionInfo> uniformBuffersInfos;
	for (uint32_t i = 0; i < paramsCount; i++)
	{
		SceGxmProgramParameter const* param = sceGxmProgramGetParameter(gxpShader, i);
		AES_LOG("param {} {}", i, sceGxmProgramParameterGetName(param));

		if (sceGxmProgramParameterGetCategory(param) == SCE_GXM_PARAMETER_CATEGORY_UNIFORM_BUFFER)
		{
			uniformBuffersInfos.push_back({ .name = std::string(sceGxmProgramParameterGetName(param)), 
					.index = i,  
					.size = sceGxmProgramParameterGetArraySize(param)}); // @Review is arraySize really what we want ?
		}
	}
	
	AES_LOG("get getUniformBufferInfos end");
	return uniformBuffersInfos;
}

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
	sceGxmShaderPatcherReleaseVertexProgram(RHIRenderContext::instance().getShaderPatcher(), vertexShader);
}

Result<void> GxmVertexShader::init(VertexShaderDescription const& desc)
{
	AES_PROFILE_FUNCTION();
	AES_ASSERT(std::holds_alternative<uint8_t*>(desc.source) && "runtime compiled shaders aren't supported on vita platform !");
	
	gxpShader = reinterpret_cast<SceGxmProgram*>(std::get<uint8_t*>(desc.source));
	AES_ASSERT(vertexShaderGxp);

	auto& context = RHIRenderContext::instance();
	auto err = sceGxmShaderPatcherRegisterProgram(context.getShaderPatcher(), gxpShader, &id);
	if (err != SCE_OK)
		return { AESError::ShaderCreationFailed };

	std::vector<SceGxmVertexAttribute> verticesAttributes(desc.verticesLayout.size());
	for (int i = 0; auto const& layout : desc.verticesLayout)
	{
		verticesAttributes[i].streamIndex = 0;
		verticesAttributes[i].offset = layout.offset;
		auto formatComponents = getFormatComponents(layout.format);
		verticesAttributes[i].format = formatComponents.attribFormat;
		verticesAttributes[i].componentCount = formatComponents.numComponents;
		verticesAttributes[i].regIndex = sceGxmProgramParameterGetResourceIndex(
				sceGxmProgramFindParameterBySemantic(gxpShader, rhiSemanticTypeToApi(layout.semantic), 0));
		i++;
	}
	
	SceGxmVertexStream vertexStream;
	vertexStream.stride = desc.verticesStride;
	vertexStream.indexSource = SCE_GXM_INDEX_SOURCE_INDEX_16BIT; // @Review

	err = sceGxmShaderPatcherCreateVertexProgram(context.getShaderPatcher(), id, verticesAttributes.data(), verticesAttributes.size(), &vertexStream, 1, &vertexShader);
	if (err != SCE_OK)
		return { AESError::ShaderCreationFailed };

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
	sceGxmShaderPatcherReleaseFragmentProgram(RHIRenderContext::instance().getShaderPatcher(), fragmentShader);
}

Result<void> GxmFragmentShader::init(FragmentShaderDescription const& desc)
{
	AES_PROFILE_FUNCTION();
	AES_ASSERT(std::holds_alternative<uint8_t*>(desc.source) && "runtime compiled shaders aren't supported on vita platform !");
	
	gxpShader = reinterpret_cast<SceGxmProgram*>(std::get<uint8_t*>(desc.source));
	AES_ASSERT(fragmentShaderGxp);

	auto& context = RHIRenderContext::instance();
	auto err = sceGxmShaderPatcherRegisterProgram(context.getShaderPatcher(), gxpShader, &id);
	if (err != SCE_OK)
		return { AESError::ShaderCreationFailed };
	
	err = sceGxmShaderPatcherCreateFragmentProgram(context.getShaderPatcher(), id, SCE_GXM_OUTPUT_REGISTER_FORMAT_UCHAR4 /* @Review */, vita_msaa_mode, nullptr, gxpShader, &fragmentShader);

	if (err != SCE_OK)
		return { AESError::ShaderCreationFailed };

	return {};
}

SceGxmFragmentProgram* GxmFragmentShader::getHandle()
{
	return fragmentShader;
}


