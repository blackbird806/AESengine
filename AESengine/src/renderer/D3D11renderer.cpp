#include "D3D11renderer.hpp"
#include "core/debug.hpp"
#include "core/maths.hpp"

#include <cstdlib>
#include <vector>

using namespace aes;

void D3D11Renderer::init(Window& window)
{
	AES_PROFILE_FUNCTION();

	renderWindow = &window;

	IDXGIFactory* factory;
	// Create a DirectX graphics interface factory.
	HRESULT result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		fatalError("failed to create CreateDXGIFactory");
	}

	// Use the factory to create an adapter for the primary graphics interface (video card).
	IDXGIAdapter* adapter;
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		fatalError("failed to create IDXGIAdapter");
	}

	// Enumerate the primary adapter output (monitor).
	IDXGIOutput* adapterOutput;
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		fatalError("failed to create IDXGIOutput");
	}

	// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
	uint numModes;
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		fatalError("failed to get DisplayModeList");
	}

	// Create a list to hold all the possible display modes for this monitor/video card combination.
	std::vector<DXGI_MODE_DESC> displayModeList(numModes);

	// Now fill the display mode list structures.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList.data());
	if (FAILED(result))
	{
		fatalError("failed to get DisplayModeList");
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
		fatalError("failed getDesc");
	}

	// Store the dedicated video card memory in megabytes.
	videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// Convert the name of the video card to a character array and store it.
	size_t stringLength;
	int error = wcstombs_s(&stringLength, videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		fatalError("failed to convert videocardName");
	}
	AES_LOG("graphic card : {}\nvideo memory : {} MB", videoCardDescription, videoCardMemory);

	// Release the adapter output.
	adapterOutput->Release();
	adapterOutput = nullptr;

	// Release the adapter.
	adapter->Release();
	adapter = nullptr;

	// Release the factory.
	factory->Release();
	factory = nullptr;

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
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// Set the usage of the back buffer.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// Set the handle for the window to render to.
	swapChainDesc.OutputWindow = window.getHandle();

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

	// Set the feature level to DirectX 11.
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_1;

	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, NULL, &deviceContext);
	if (FAILED(result))
	{
		fatalError("D3D11CreateDeviceAndSwapChain failed");
	}

	// Get the pointer to the back buffer.
	ID3D11Texture2D* backBufferPtr;
	result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		fatalError("swapChain->GetBuffer failed");
	}

	result = device->CreateRenderTargetView(backBufferPtr, NULL, &renderTargetView);
	if (FAILED(result))
	{
		fatalError("device->CreateRenderTargetView failed");
	}

	backBufferPtr->Release();
	backBufferPtr = nullptr;

	// Initialize the description of the depth buffer.
	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
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

	result = device->CreateTexture2D(&depthBufferDesc, NULL, &depthStencilBuffer);
	if (FAILED(result))
	{
		fatalError("device->CreateTexture2D failed");
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
		fatalError("device->CreateDepthStencilState failed");
	}

	// Set the depth stencil state.
	deviceContext->OMSetDepthStencilState(depthStencilState, 1);

	// Initailze the depth stencil view.
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = device->CreateDepthStencilView(depthStencilBuffer, &depthStencilViewDesc, &depthStencilView);
	if (FAILED(result))
	{
		fatalError("device->CreateDepthStencilView failed");
	}

	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

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

	result = device->CreateRasterizerState(&rasterDesc, &rasterState);
	if (FAILED(result))
	{
		fatalError("device->rasterState failed");
	}

	deviceContext->RSSetState(rasterState);

	D3D11_VIEWPORT viewport;
	// Setup the viewport for rendering.
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	deviceContext->RSSetViewports(1, &viewport);

	AES_LOG("D3D11 renderer initialized");

	model.init(device);
	shader.init(device);

	cam.pos = {0.0, 0.0, 1.0};
	cam.lookAt({0.0, 0.0, -1.0});

	AES_LOG("D3D11 debugs initialized");
}

void D3D11Renderer::destroy()
{
	AES_PROFILE_FUNCTION();

	AES_ASSERT(swapChain != nullptr);
	AES_ASSERT(rasterState != nullptr);
	AES_ASSERT(depthStencilView != nullptr);
	AES_ASSERT(depthStencilState != nullptr);
	AES_ASSERT(depthStencilBuffer != nullptr);
	AES_ASSERT(renderTargetView != nullptr);
	AES_ASSERT(deviceContext != nullptr);
	AES_ASSERT(device != nullptr);
	AES_ASSERT(swapChain != nullptr);

	// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
	swapChain->SetFullscreenState(false, NULL);
	rasterState->Release();
	depthStencilView->Release();
	depthStencilState->Release();
	depthStencilBuffer->Release();
	renderTargetView->Release();
	deviceContext->Release();
	device->Release();
	swapChain->Release();
}

void D3D11Renderer::startFrame()
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
	
	//pmat = makePerspectiveMatrix(60, 0.001, 1000);
	Eigen::Matrix4f pmat = makeOrthographicMatrix(-1.0, 1.0, 1.0, -1.0, 0.01, 1000);
	shader.render(deviceContext);
	model.render(deviceContext);
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

