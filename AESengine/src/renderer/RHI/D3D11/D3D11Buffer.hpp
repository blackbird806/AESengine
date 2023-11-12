#ifndef AES_D3D11BUFFER_HPP
#define AES_D3D11BUFFER_HPP

#include <d3d11.h>

#include "renderer/RHI/RHIElements.hpp"
#include "core/error.hpp"

struct ID3D11Buffer;
namespace aes
{
	class D3D11Buffer
	{
	public:
		friend class D3D11Device;

		D3D11Buffer() = default;
		D3D11Buffer(D3D11Buffer&&) noexcept;
		D3D11Buffer& operator=(D3D11Buffer&& rhs) noexcept;
		void destroy() noexcept;
		~D3D11Buffer();
		
		ID3D11Buffer* getHandle() noexcept;
		
		size_t getSize() const;
		bool isValid() const;
		
	protected:

		ID3D11Buffer* apiBuffer = nullptr;
		size_t size = 0;
	};
	
	using RHIBufferBase = D3D11Buffer;
}

#endif
