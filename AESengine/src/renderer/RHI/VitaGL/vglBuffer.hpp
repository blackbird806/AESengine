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
		~VglBuffer();

		Result<void> create(BufferDescription const& desc)
		{
			glGenBuffers(1, &apiBuffer);
		}

		Result<void> bind(uint slot);
		Result<void> unbind();
		Result<void*> map();
		Result<void> unmap();

	private:

		BindFlags bindFlags;
		GLuint apiBuffer;
		size_t size;
	};

	using RHIBufferBase = VglBuffer;
}
#endif
