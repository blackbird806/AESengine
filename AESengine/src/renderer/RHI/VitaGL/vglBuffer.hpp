#ifndef VGLBUFFER_HPP
#define VGLBUFFER_HPP

#include <vitaGL.h>

#include "renderer/RHI/RHIElements.hpp"
#include "core/error.hpp"

namespace aes
{
	class VglBuffer
	{
		
	public:

		VglBuffer() {}
		VglBuffer(VglBuffer&&) noexcept;
		VglBuffer& operator=(VglBuffer&& rhs) noexcept;
		~VglBuffer();

		Result<void> create(BufferDescription const& desc);
		Result<void*> map();
		Result<void> unmap();

		GLuint getHandle();

	private:

		BindFlags bindFlags;
		GLuint apiBuffer;
		size_t size;
	};

	using RHIBufferBase = VglBuffer;
}
#endif
