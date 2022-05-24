#ifndef AES_RHIELEMENTS_HPP
#define AES_RHIELEMENTS_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <span>
#include <variant>

#include "core/aes.hpp"
#include "core/utility.hpp"

namespace aes
{
	enum CPUAccessFlagBits : uint8_t
	{
		None = 0x0,
		Read = 0x1,
		Write = 0x2
	};
	using CPUAccessFlags = Flags<CPUAccessFlagBits>;

	enum class MemoryUsage
	{
		Default,
		Immutable,	// GPU Readonly, hidden from CPU
		Dynamic,	// GPU Readonly, CPU WriteOnly
		Staging
	};

	enum class BindFlagBits
	{
		VertexBuffer = 0x1,
		IndexBuffer = 0x2,
		UniformBuffer = 0x4,
	};
	using BindFlags = Flags<BindFlagBits>;

	enum class BlendOp
	{
		Add,
		Sub
	};

	enum class BlendFactor
	{
		Zero,
		One,
		SrcColor,
		SrcAlpha,
		DstColor,
		DstAlpha,
		OneMinusSrcColor,
		OneMinusDstColor,
	};

	enum class DrawPrimitiveType
	{
		Lines,
		LineStrip,
		Triangles,
		TriangleStrip,
		Points,
	};

	enum class IndexTypeFormat
	{
		Uint16,
		Uint32,
	};

	enum class SemanticType
	{
		Position,
		Normal,
		Color,
		TexCoord
	};

	enum class TextureAddressMode
	{
		Repeat,
		Mirror,
		Clamp,
	};

	enum class TextureFilter
	{
		Point,
		Linear,
	};

	constexpr const char* getSemanticName(SemanticType e)
	{
		switch (e)
		{
		case SemanticType::Position: return "POSITION";
		case SemanticType::Normal: return "NORMAL";
		case SemanticType::Color: return "COLOR";
		case SemanticType::TexCoord: return "TEXCOORD";
		}
		AES_UNREACHABLE();
	}

	struct BufferDescription
	{
		size_t sizeInBytes;
		MemoryUsage usage;
		BindFlags bindFlags;
		CPUAccessFlags cpuAccessFlags;
		void* initialData = nullptr;
	};

	struct SamplerDescription
	{
		TextureFilter filter;
		TextureAddressMode addressU;
		TextureAddressMode addressV;
		uint32_t lodMin;
		uint32_t lodBias;
	};

	enum class RHIFormat
	{
		R8G8B8A8_Uint,
		R16G16_Float,
		R32G32_Float,

		R32G32B32_Float,

		R16G16B16A16_Float,
		R32G32B32A32_Float,
	};

	constexpr uint getFormatSize(RHIFormat format)
	{
		switch(format)
		{
		case RHIFormat::R8G8B8A8_Uint: return 4;
		case RHIFormat::R16G16_Float: return 4;
		case RHIFormat::R32G32_Float: return 8;
		case RHIFormat::R32G32B32_Float: return 12;
		case RHIFormat::R16G16B16A16_Float: return 8;
		case RHIFormat::R32G32B32A32_Float: return 16;
		}
		AES_UNREACHABLE();
	}

	struct VertexInputLayout
	{
		// @Review use semantic for gxm too ?
		SemanticType semantic;		// used only by d3d11
		const char* parameterName;	// used only by gxm
		RHIFormat format;
		uint32_t offset;
	};

	struct ShaderDescription
	{
		using Binary = uint8_t const*;
		std::variant<std::string, Binary> source;
	};

	struct VertexShaderDescription : ShaderDescription
	{
		std::span<VertexInputLayout> verticesLayout;
		uint32_t verticesStride;
	};

	struct FragmentShaderDescription : ShaderDescription
	{
		// TODO: clean this
		void const* gxpVertexProgram; 
	};

	struct TextureDescription
	{
		uint width, height;
		uint mipsLevel;
		RHIFormat format;
		MemoryUsage usage;
		CPUAccessFlags cpuAccess;
		void* initialData = nullptr;
	};

	// theses functions assert if structure is invalid according to RHI
	// furthers assertions can still be triggered for platform/API specific constraints
	void validateVertexShaderDescription(VertexShaderDescription const& desc);
	void validateTextureDescription(TextureDescription const& desc);
	void validateBufferDescription(BufferDescription const& desc);
}


#endif
