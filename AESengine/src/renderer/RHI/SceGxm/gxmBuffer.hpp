#ifndef GXMBUFFER_HPP
#define GXMBUFFER_HPP

#include "renderer/RHI/RHIElements.hpp"
#include "core/error.hpp"

namespace aes
{
	class GxmBuffer
	{
		
	public:

		GxmBuffer() {}
		GxmBuffer(GxmBuffer&&) noexcept;
		GxmBuffer& operator=(GxmBuffer&& rhs) noexcept;
		~GxmBuffer();

		Result<void> create(BufferDescription const& desc);
		Result<void*> map();
		Result<void> unmap();

	private:

		BindFlags bindFlags;
		size_t size;
	};

	using RHIBufferBase = GxmBuffer;
}
#endif
