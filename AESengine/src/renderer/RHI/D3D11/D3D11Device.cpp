#include "D3D11Device.hpp"
#include <dxgi.h>
#include "D3D11Elements.hpp"
#include "core/aesException.hpp"
#include "D3D11shader.hpp"
#include "renderer/RHI/RHIBuffer.hpp"

using namespace aes;

void aes::initializeGraphicsAPI()
{
}

void aes::terminateGraphicsAPI()
{
}

Result<void> D3D11Device::init()
{
	AES_PROFILE_FUNCTION();

	// Create a DirectX graphics interface factory.
	HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		AES_FATAL_ERROR("failed to create CreateDXGIFactory");
	}

	// Use the factory to create an adapter for the primary graphics interface (video card).
	IDXGIAdapter* adapter;
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		AES_FATAL_ERROR("failed to create IDXGIAdapter");
	}

	// Enumerate the primary adapter output (monitor).
	IDXGIOutput* adapterOutput;
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		AES_FATAL_ERROR("failed to create IDXGIOutput");
	}

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	uint numModes;
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, nullptr);
	if (FAILED(result))
	{
		AES_FATAL_ERROR("failed to get DisplayModeList");
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	std::vector<DXGI_MODE_DESC> displayModeList(numModes);

	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList.data());
	if (FAILED(result))
	{
		AES_FATAL_ERROR("failed to get DisplayModeList");
	}

	// Get the adapter (video card) description.
	DXGI_ADAPTER_DESC adapterDesc;
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		AES_FATAL_ERROR("failed getDesc");
	}

	// Store the dedicated video card memory in megabytes.
	int const videoCardMemory = static_cast<int>(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	size_t stringLength;
	char videoCardDescription[128];
	int const error = wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		AES_FATAL_ERROR("failed to convert videocardName");
	}
	AES_LOG("graphic card : {}\nvideo memory : {} MB", videoCardDescription, videoCardMemory);

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = nullptr;

	// Release the adapter.
	adapter->Release();
	adapter = nullptr;

	// Set the feature level to DirectX 11.
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
	result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
#ifdef AES_DEBUG
		D3D11_CREATE_DEVICE_DEBUG,
#else
		0,
#endif
		& featureLevel, 1, D3D11_SDK_VERSION, &device, nullptr, &deviceContext);
	if (FAILED(result))
	{
		AES_FATAL_ERROR("failed to create D3D11 device");
	}

#ifdef AES_DEBUG
	result = device->QueryInterface(IID_PPV_ARGS(&debugInterface));
	if (FAILED(result))
	{
		AES_FATAL_ERROR("failed to query debug interface");
	}
#endif
	return {};
}

void D3D11Device::destroy()
{
	AES_PROFILE_FUNCTION();

	AES_ASSERT(deviceContext != nullptr);
	AES_ASSERT(factory != nullptr);
	AES_ASSERT(device != nullptr);
	AES_ASSERT(debugInterface != nullptr);

	deviceContext->Release();
	factory->Release();
	device->Release();

	device = nullptr;
	factory = nullptr;
	deviceContext = nullptr;
#ifdef AES_DEBUG
	debugInterface->Release();
	debugInterface = nullptr;
#endif
}

D3D11Device::D3D11Device(D3D11Device&& rhs) noexcept
{
	*this = std::move(rhs);
}

D3D11Device::~D3D11Device()
{
	if (device != nullptr)
	{
		destroy();
	}
}

D3D11Device& D3D11Device::operator=(D3D11Device&& rhs) noexcept
{
	destroy();
	device = rhs.device;
	factory = rhs.factory;
	deviceContext = rhs.deviceContext;

	rhs.device = nullptr;
	rhs.factory = nullptr;
	rhs.deviceContext = nullptr;

#ifdef AES_DEBUG
	debugInterface = rhs.debugInterface;
	rhs.debugInterface = nullptr;
#endif
	return *this;
}

void D3D11Device::drawIndexed(uint indexCount, uint indexOffset)
{
	AES_PROFILE_FUNCTION();
	deviceContext->DrawIndexed(indexCount, indexOffset, 0);
}

void D3D11Device::setCullMode(CullMode mode)
{
	AES_PROFILE_FUNCTION();
	rasterStateDesc.CullMode = rhiCullModeToApi(mode);
	setRasterizerState();
}

void D3D11Device::setDrawPrimitiveMode(DrawPrimitiveType mode)
{
	AES_PROFILE_FUNCTION();
	deviceContext->IASetPrimitiveTopology(rhiPrimitiveTypeToApi(mode));
}

void D3D11Device::setRasterizerState()
{
	HRESULT const result = device->CreateRasterizerState(&rasterStateDesc, &rasterState);
	if (FAILED(result))
	{
		throw RHIException("D3D11 CreateRasterizerState failed !");
	}
	deviceContext->RSSetState(rasterState);
}


void D3D11Device::setBlendState(D3D11BlendState& blendState)
{
	AES_PROFILE_FUNCTION();
	deviceContext->OMSetBlendState(blendState.getHandle(), nullptr, 0xffffffff);
}

void D3D11Device::setFragmentShader(RHIFragmentShader& fs)
{
	AES_PROFILE_FUNCTION();
	if (fs.blendState.getHandle() != nullptr)
		setBlendState(fs.blendState);
	deviceContext->PSSetShader(fs.getHandle(), nullptr, 0);
}

void D3D11Device::setVertexShader(RHIVertexShader& vs)
{
	AES_PROFILE_FUNCTION();
	deviceContext->IASetInputLayout(vs.getInputLayout());
	deviceContext->VSSetShader(vs.getHandle(), nullptr, 0);
}

Result<void> D3D11Device::setVertexBuffer(RHIBuffer& buffer, uint stride, uint offset)
{
	AES_PROFILE_FUNCTION();
	ID3D11Buffer* handle = buffer.getHandle();
	deviceContext->IASetVertexBuffers(0, 1, &handle, &stride, &offset);
	return {};
}

Result<void> D3D11Device::setIndexBuffer(RHIBuffer& buffer, IndexTypeFormat typeFormat, uint offset)
{
	AES_PROFILE_FUNCTION();
	deviceContext->IASetIndexBuffer(buffer.getHandle(), rhiTypeFormatToApi(typeFormat), offset);
	return {};
}
