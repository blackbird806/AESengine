#ifndef WOB_ERROR_CODES
#define WOB_ERROR_CODES

#include <cstdint>
namespace wob {

	using UnderlyingError_t = int32_t;

	enum class AESError : UnderlyingError_t
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
}

#endif