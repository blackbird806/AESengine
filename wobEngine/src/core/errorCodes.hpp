#ifndef WOB_ERROR_CODES
#define WOB_ERROR_CODES

#include <stdint.h>

namespace wob 
{
	using UnderlyingError_t = int64_t;

	enum class ErrorCode : UnderlyingError_t
	{
		Undefined,
		MemoryAllocationFailed,
		RHIDeviceCreationFailed,
		GPUBufferCreationFailed,
		GPUTextureCreationFailed,
		GPUBufferMappingFailed,
		SamplerCreationFailed,
		ShaderCompilationFailed,
		ShaderCreationFailed,
		FontInitFailed,
		BlendStateCreationFailed,
		RenderTargetCreationFailed,
		SamplerApplicationFailed
	};

	inline const char* to_string(ErrorCode err)
	{
#define CASE(X) case ErrorCode::X: return #X;
		switch (err)
		{
			CASE(Undefined)
				CASE(MemoryAllocationFailed)
				CASE(GPUBufferCreationFailed)
				CASE(GPUTextureCreationFailed)
				CASE(GPUBufferMappingFailed)
				CASE(SamplerCreationFailed)
				CASE(ShaderCompilationFailed)
				CASE(ShaderCreationFailed)
				CASE(FontInitFailed)
				CASE(BlendStateCreationFailed)
				CASE(RenderTargetCreationFailed)
				CASE(SamplerApplicationFailed)
		}
#undef CASE
		return "undefined";
	}
}

#endif