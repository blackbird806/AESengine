#ifndef AES_D3D11DEVICE_HPP
#define AES_D3D11DEVICE_HPP

#include "core/error.hpp"

struct IDXGIFactory;
struct ID3D11Device;
struct ID3D11Debug;
struct ID3D11DeviceContext;

namespace aes
{
	class D3D11Device
	{
	public:
		D3D11Device() = default;
		D3D11Device(D3D11Device const&) = delete;
		D3D11Device(D3D11Device&&) noexcept;
		~D3D11Device();

		D3D11Device& operator=(D3D11Device&&) noexcept;

		Result<void> init();
		void destroy();

	private:
		IDXGIFactory* factory = nullptr;
		ID3D11Device* device = nullptr;
		ID3D11DeviceContext* deviceContext = nullptr;
#ifdef AES_DEBUG
		ID3D11Debug* debugInterface = nullptr;
#endif
	};

	using RHIDevice = D3D11Device;
}

#endif