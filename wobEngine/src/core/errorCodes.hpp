#ifndef AES_ERROR_CODES
#define AES_ERROR_CODES

#include <cstdint>
namespace aes {

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