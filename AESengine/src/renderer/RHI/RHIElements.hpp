#ifndef RHI_ELEMENTS_HPP
#define RHI_ELEMENTS_HPP

#include <cstddef>
#include <cstdint>

namespace aes
{
	enum class CPUAccessFlags : uint8_t
	{
		None = 0x0,
		Read = 0x1,
		Write = 0x2
	};

	enum class GPUAccessFlags : uint8_t
	{
		None = 0x0,
		Read = 0x1,
		Write = 0x2
	};

	enum class Usage
	{
		Default,
		Immutable,
		Dynamic,
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
		Usage bufferUsage;
		BindFlags bindFlags;
		uint8_t cpuAccessFlags = (uint8_t)CPUAccessFlags::None;
		uint8_t gpuAccessFlags = (uint8_t)GPUAccessFlags::None;
		void* initialData = nullptr;
	};

}

#endif
