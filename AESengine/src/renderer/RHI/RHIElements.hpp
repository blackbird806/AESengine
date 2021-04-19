#ifndef RHI_ELEMENTS_HPP
#define RHI_ELEMENTS_HPP

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <span>
#include <variant>

#include "core/aes.hpp"

namespace aes
{
	enum class CPUAccessFlags : uint8_t
	{
		None = 0x0,
		Read = 0x1,
		Write = 0x2
	};

	enum class BufferUsage
	{
		Default,
		Immutable,	// GPU Readonly, hidden from CPU
		Dynamic,	// GPU Readonly, CPU WriteOnly
		Staging
	};

	enum class BindFlags
	{
		VertexBuffer,
		IndexBuffer,
		UniformBuffer,
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

	inline const char* getSemanticName(SemanticType e)
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
		BufferUsage bufferUsage;
		BindFlags bindFlags;
		uint8_t cpuAccessFlags;
		void* initialData = nullptr;
	};

	enum class RHIFormat
	{
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
		std::variant<std::string_view, uint8_t*> source;
	};

	struct VertexShaderDescription : public ShaderDescription
	{
		std::span<VertexInputLayout> verticesLayout;
		uint32_t verticesStride;
	};

	struct FragmentShaderDescription : public ShaderDescription
	{
		
	};	
}

#endif
