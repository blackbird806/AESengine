#include "D3D11renderer.hpp"
#include "core/debug.hpp"
#include "renderer/RHI/RHIBuffer.hpp"
#include "renderer/RHI/RHIElements.hpp"

#include <vector>

#include "D3D11Elements.hpp"

using namespace aes;

D3D11Renderer& D3D11Renderer::instance()
{
	static D3D11Renderer instance;
	return instance;
}

void D3D11Renderer::init(Window& window)
{
	AES_PROFILE_FUNCTION();

	renderWindow = &window;
	window.setResizeCallback([](uint, uint) {
			instance().resizeRender();
		});

	// Create a DirectX graphics interface factory.
	HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		AES_ERROR("failed to create CreateDXGIFactory");
	}

	// Use the factory to create an adapter for the primary graphics interface (video card).
	IDXGIAdapter* adapter;
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		AES_ERROR("failed to create IDXGIAdapter");
	}

	// Enumerate the primary adapter output (monitor).
	IDXGIOutput* adapterOutput;
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		AES_ERROR("failed to create IDXGIOutput");
	}

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	uint numModes;
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		AES_ERROR("failed to get DisplayModeList");
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	std::vector<DXGI_MODE_DESC> displayModeList(numModes);

	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList.data());
	if (FAILED(result))
	{
		AES_ERROR("failed to get DisplayModeList");
	}

	// Now go through all the display modes and find the one that matches the screen width and height.
	// When a match is found store the numerator and denominator of the refresh rate for that monitor.
	uint screenWidth, screenHeight;
	window.getScreenSize(screenWidth, screenHeight);
	uint numerator, denominator;
	for (uint i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == screenWidth)
		{
			if (displayModeList[i].Height == screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// Get the adapter (video card) description.
	DXGI_ADAPTER_DESC adapterDesc;
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		AES_ERROR("failed getDesc");
	}

	// Store the dedicated video card memory in megabytes.
	videoCardMemory = static_cast<int>(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	size_t stringLength;
	int const error = wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		AES_ERROR("failed to convert videocardName");
	}
	AES_LOG("graphic card : {}\nvideo memory : {} MB", videoCardDescription, videoCardMemory);

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = nullptr;

	// Release the adapter.
	adapter->Release();
	adapter = nullptr;

	createDevice();
	createSwapchain();
	createRenderTarget();
	createDepthStencil();
	setupRasterizerState();
	setupViewport();

	AES_LOG("D3D11 renderer initialized");

	shader.init();

	AES_LOG("D3D11 debugs initialized");
}

void D3D11Renderer::destroy()
{
	AES_PROFILE_FUNCTION();

	AES_ASSERT(rasterState != nullptr);
	AES_ASSERT(renderTargetView != nullptr);
	AES_ASSERT(deviceContext != nullptr);
	AES_ASSERT(factory != nullptr);
	AES_ASSERT(device != nullptr);

	rasterState->Release();
	destroyDepthStencil();
	destroyRenderTarget();
	deviceContext->Release();
	factory->Release();
	device->Release();
	destroySwapchain();
}

void D3D11Renderer::bindBuffer(RHIBuffer& buffer, uint slot)
{
	ID3D11Buffer* handle = buffer.getHandle();
	deviceContext->VSSetConstantBuffers(slot, 1, &handle);
}

void D3D11Renderer::bindVertexBuffer(RHIBuffer& buffer, uint stride, uint offset)
{
	ID3D11Buffer* handle = buffer.getHandle();
	deviceContext->IASetVertexBuffers(0, 1, &handle, &stride, &offset);
}

void D3D11Renderer::bindIndexBuffer(RHIBuffer& buffer, TypeFormat typeFormat, uint offset)
{
	deviceContext->IASetIndexBuffer(buffer.getHandle(), rhiTypeFormatToApi(typeFormat), offset);
}

void D3D11Renderer::setDrawPrimitiveMode(DrawPrimitiveMode mode)
{
	deviceContext->IASetPrimitiveTopology(rhiPrimitiveModeToApi(mode));
}

ID3D11Device* D3D11Renderer::getDevice()
{
	return device;
}

ID3D11DeviceContext* D3D11Renderer::getDeviceContext()
{
	return deviceContext;
}

void D3D11Renderer::drawIndexed(uint indexCount)
{
	deviceContext->DrawIndexed(indexCount, 0, 0);
}

void D3D11Renderer::startFrame(Camera const& cam)
{
	AES_PROFILE_FUNCTION();

	float color[4];

	// Setup the color to clear the buffer to.
	color[0] = 0.0f;
	color[1] = 0.0f;
	color[2] = 0.0f;
	color[3] = 1.0f;

	// Clear the back buffer.
	deviceContext->ClearRenderTargetView(renderTargetView, color);

	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	shader.render(cam.viewMatrix, cam.projMatrix);
}

void D3D11Renderer::endFrame()
{
	AES_PROFILE_FUNCTION();

	// Present the back buffer to the screen since rendering is complete.
	if (vsyncEnabled)
	{
		// Lock to screen refresh rate.
		swapChain->Present(1, 0);
	}
	else
	{
		// Present as fast as possible.
		swapChain->Present(0, 0);
	}
}

//https://seanmiddleditch.com/direct3d-11-debug-api-tricks/
void D3D11Renderer::createDevice()
{
	AES_PROFILE_FUNCTION();
	
	// Set the feature level to DirectX 11.
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;
	auto result = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
#ifdef AES_DEBUG
		D3D11_CREATE_DEVICE_DEBUG,
#else
		0,
#endif
		&featureLevel, 1, D3D11_SDK_VERSION, &device, NULL, &deviceContext);
	if (FAILED(result))
	{
		AES_ERROR("failed to create D3D11 device");
	}
	
#ifdef AES_DEBUG
	result = device->QueryInterface(IID_PPV_ARGS(&debugInterface));
	if (FAILED(result))
	{
		AES_ERROR("failed to query debug interface");
	}
#endif
}

void D3D11Renderer::createSwapchain()
{
	AES_PROFILE_FUNCTION();

	uint screenWidth, screenHeight;
	renderWindow->getScreenSize(screenWidth, screenHeight);

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

	// Set to a single back buffer.
	swapChainDesc.BufferCount = 1;

	// Set the width and height of the back buffer.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// Set regular 32-bit surface for the back buffer.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// Set the refresh rate of the back buffer.
	if (vsyncEnabled)
	{
		AES_ERROR("Not implemented");
		//swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		//swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = (HWND)renderWindow->getHandle();

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// Set to full screen or windowed mode.
	swapChainDesc.Windowed = true;

	// Set the scan line ordering and scaling to unspecified.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Discard the back buffer contents after presenting.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching

	AES_ASSERT(factory);
	HRESULT result = factory->CreateSwapChain(device, &swapChainDesc, &swapChain);
	if (FAILED(result))
	{
		AES_ERROR("D3D11CreateDeviceAndSwapChain failed");
	}
}

void D3D11Renderer::createRenderTarget()
{
	AES_PROFILE_FUNCTION();

	// Get the pointer to the back buffer.
	ID3D11Texture2D* backBufferPtr;
	HRESULT result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		AES_ERROR("swapChain->GetBuffer failed");
	}
	
	result = device->CreateRenderTargetView(backBufferPtr, NULL, &renderTargetView);
	if (FAILED(result))
	{
		AES_ERROR("device->CreateRenderTargetView failed");
	}

	backBufferPtr->Release();
	backBufferPtr = nullptr;
}

void D3D11Renderer::createDepthStencil()
{
	AES_PROFILE_FUNCTION();

	uint screenWidth, screenHeight;
	renderWindow->getScreenSize(screenWidth, screenHeight);
	
	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	// Set up the description of the depth buffer.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	auto result = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);
	if (FAILED(result))
	{
		AES_ERROR("device depthStencilBuffer failed");
	}

	// Initialize the description of the stencil state.
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	result = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	if (FAILED(result))
	{
		AES_ERROR("device->CreateDepthStencilState failed");
	}

	deviceContext->OMSetDepthStencilState(depthStencilState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
	if (FAILED(result))
	{
		AES_ERROR("device->CreateDepthStencilView failed");
	}

	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
}

void D3D11Renderer::setupRasterizerState()
{
	AES_PROFILE_FUNCTION();

	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	HRESULT const result = device->CreateRasterizerState(&rasterDesc, &rasterState);
	if (FAILED(result))
	{
		AES_ERROR("device->rasterState failed");
	}
	deviceContext->RSSetState(rasterState);
}

void D3D11Renderer::setupViewport()
{
	AES_PROFILE_FUNCTION();

	uint screenWidth, screenHeight;
	renderWindow->getScreenSize(screenWidth, screenHeight);

	D3D11_VIEWPORT viewport;
	// Setup the viewport for rendering.
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	deviceContext->RSSetViewports(1, &viewport);
}

void D3D11Renderer::resizeRender()
{
	AES_PROFILE_FUNCTION();

	destroyDepthStencil();
	destroyRenderTarget();
	destroySwapchain();

	createSwapchain();
	createRenderTarget();
	createDepthStencil();
	setupViewport();
}

void D3D11Renderer::destroySwapchain()
{
	AES_PROFILE_FUNCTION();

	AES_ASSERT(swapChain != nullptr);
	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	swapChain->SetFullscreenState(false, NULL);
	swapChain->Release();
	swapChain = nullptr;
}

void D3D11Renderer::destroyDepthStencil()
{
	AES_PROFILE_FUNCTION();

	AES_ASSERT(depthStencilView != nullptr);
	AES_ASSERT(depthStencilState != nullptr);
	AES_ASSERT(depthStencilBuffer != nullptr);
	depthStencilView->Release();
	depthStencilState->Release();
	depthStencilBuffer->Release();
	depthStencilView = nullptr;
	depthStencilState = nullptr;
	depthStencilBuffer = nullptr;
}

void D3D11Renderer::destroyRenderTarget()
{
	AES_PROFILE_FUNCTION();

	AES_ASSERT(renderTargetView);
	renderTargetView->Release();
}