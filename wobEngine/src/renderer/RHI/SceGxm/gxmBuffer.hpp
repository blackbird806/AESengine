#ifndef WOB_GXMBUFFER_HPP
#define WOB_GXMBUFFER_HPP

#include "renderer/RHI/RHIElements.hpp"
#include "core/error.hpp"
#include <psp2/gxm.h>

namespace wob
{
	class GxmBuffer
	{
		
	public:

		GxmBuffer() {}
		GxmBuffer(GxmBuffer&&) noexcept;
		GxmBuffer& operator=(GxmBuffer&& rhs) noexcept;
		~GxmBuffer();

		void destroy() noexcept;

		bool isValid() const;
		size_t getSize() const;

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
