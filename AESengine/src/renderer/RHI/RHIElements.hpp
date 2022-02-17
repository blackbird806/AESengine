#ifndef RHI_ELEMENTS_HPP
#define RHI_ELEMENTS_HPP

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

	enum class BufferUsage
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

	enum class TypeFormat
	{
		Float16,
		Float32,

		Uint8,
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
		BufferUsage bufferUsage; // @TODO rename usage
		BindFlags bindFlags;
		CPUAccessFlags cpuAccessFlags;
		void* initialData = nullptr;
	};

	enum class RHIFormat
	{
		U8n,
		R16G16_Float,
		R32G32_Float,
		
		R32G32B32_Float,

		R16G16B16A16_Float,
		R32G32B32A32_Float,
	};
	
	struct VertexInputLayout
	{
		SemanticType semantic;
		RHIFormat format;
		uint32_t offset;
	};

	struct ShaderDescription
	{
		using Binary = uint8_t const*;
		std::variant<std::string, Binary> source;
	};

	struct VertexShaderDescription : public ShaderDescription
	{
		std::span<VertexInputLayout> verticesLayout;
		uint32_t verticesStride;
	};

	struct FragmentShaderDescription : public ShaderDescription
	{
		// TODO: clean this
		void const* gxpVertexProgram; 
	};
}

#endif
