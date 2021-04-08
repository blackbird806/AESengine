#ifndef RHI_ELEMENTS_HPP
#define RHI_ELEMENTS_HPP

#include <cstddef>

namespace aes
{
	enum class CPUAccessFlags
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
		CPUAccessFlags cpuAccessFlags = CPUAccessFlags::None;
		void* initialData = nullptr;
	};

}

#endif
