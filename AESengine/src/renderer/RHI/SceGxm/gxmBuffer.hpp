#ifndef GXMBUFFER_HPP
#define GXMBUFFER_HPP

#include "renderer/RHI/RHIElements.hpp"
#include "core/error.hpp"
#include <psp2/gxm.h>

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
		
		void* getHandle();

	private:

		BindFlags bindFlags;
		SceUID memID;
		void* buffer;
		size_t size;
	};

	using RHIBufferBase = GxmBuffer;
}
#endif
