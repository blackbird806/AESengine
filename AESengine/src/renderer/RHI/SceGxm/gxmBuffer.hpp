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

		Result<void> init(BufferDescription const& desc);
		Result<void*> map();
		Result<void> unmap();
		
		bool isValid() const;
		size_t getSize() const;
		Result<void> copyTo(GxmBuffer& dest);

		void* getHandle();

	private:

		BindFlags bindFlags;
		SceUID memID;
		void* buffer = nullptr;
		size_t size;
	};

	using RHIBufferBase = GxmBuffer;
}
#endif
