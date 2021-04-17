#ifndef D3D11BUFFER_HPP
#define D3D11BUFFER_HPP

#include <d3d11.h>

#include "renderer/RHI/RHIElements.hpp"
#include "core/error.hpp"

namespace aes
{
	class D3D11Buffer
	{
	public:
		
		D3D11Buffer() {}
		D3D11Buffer(D3D11Buffer&&) noexcept;
		D3D11Buffer& operator=(D3D11Buffer&& rhs) noexcept;
		~D3D11Buffer();
		
		Result<void> create(BufferDescription const& desc);
		Result<void> copyTo(D3D11Buffer& dest);
		
		ID3D11Buffer* getHandle() noexcept;
		Result<void*> map();
		Result<void> unmap();
		
		size_t getSize() const;
		bool isValid() const;
		
	protected:

		BindFlags bindFlags;
		ID3D11Buffer* apiBuffer;
		size_t size = 0;
	};
	
	using RHIBufferBase = D3D11Buffer;
}

#endif
