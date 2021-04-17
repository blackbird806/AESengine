#ifndef RHI_ELEMENTS_HPP
#define RHI_ELEMENTS_HPP

#include <cstddef>
#include <cstdint>
#include <string>

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
		Uint32,
		Uint16,
		Uint8,
	};

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
		R8G8_FLOAT,
		R16G16_FLOAT,
		R32G32_FLOAT,
		
		R8G8B8_FLOAT,
		R16G16B16_FLOAT,
		R32G32B32_FLOAT,

		R8G8B8A8_FLOAT,
		R16G16B16A16_FLOAT,
		R32G32B32A32_FLOAT,
	};
	
	struct VertexInputBindingDescription
	{
		std::string name;
		uint32_t stride;
	};

	struct VertexInputAttributeDescription
	{
		RHIFormat format;
		uint32_t offset;
	};

	struct ShaderDescription
	{
		std::string source;
	};
}

#endif
