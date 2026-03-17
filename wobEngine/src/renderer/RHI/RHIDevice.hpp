#ifndef WOB_RHIDEVICE_HPP
#define WOB_RHIDEVICE_HPP

#include "renderer/RHI/RHI.hpp"

#ifdef WOB_GRAPHIC_API_D3D11
	#include "renderer/RHI/D3D11/D3D11Device.hpp"
#elif defined(WOB_GRAPHIC_API_GXM)
	#include "renderer/RHI/SceGxm/gxmDevice.hpp"
#endif

namespace wob 
{
	class RHIDevice : public RHIDeviceBase
	{
	public:
		Result<void> setBufferData(RHIBuffer const& buffer, void* data, size_t size);
		Result<void> reallocBuffer(RHIBuffer& buffer, BufferDescription const& reallocDesc);
		Result<void> ensureBufferCapacity(RHIBuffer& buffer, BufferDescription const& reallocDesc);
	};
}

#endif