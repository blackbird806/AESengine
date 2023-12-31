#include "D3D11Device.hpp"
#include "D3D11Elements.hpp"
#include "core/aesException.hpp"
#include "D3D11shader.hpp"
#include "renderer/RHI/RHIBuffer.hpp"
#include "core/array.hpp"
#include <dxgi.h>
#include <d3dcompiler.h>

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

Result<RHISwapchain> D3D11Device::createSwapchain(SwapchainDescription const& desc)
{
	AES_PROFILE_FUNCTION();

	RHISwapchain sc;
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

	swapChainDesc.BufferCount = desc.count;

	swapChainDesc.BufferDesc.Width = desc.width;
	swapChainDesc.BufferDesc.Height = desc.height;

	swapChainDesc.BufferDesc.Format = rhiFormatToApi(desc.format);

	// Set the refresh rate of the back buffer.
	//if (vsyncEnabled)
	//{
	//	AES_NOT_IMPLEMENTED();
	//	//swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
	//	//swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	//}
	//else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	swapChainDesc.OutputWindow = static_cast<HWND>(desc.window);

	// Turn multisampling off.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	swapChainDesc.Windowed = true;

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // allow full-screen switching

	AES_ASSERT(factory);
	HRESULT result = factory->CreateSwapChain(device, &swapChainDesc, &sc.swapchain);
	if (FAILED(result))
	{
		AES_FATAL_ERROR("D3D11CreateDeviceAndSwapChain failed");
	}

	result = sc.swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&sc.rt));
	if (FAILED(result))
	{
		AES_FATAL_ERROR("swapChain->GetBuffer failed");
	}

	result = device->CreateRenderTargetView(sc.rt, nullptr, &sc.rtview);
	if (FAILED(result))
	{
		AES_FATAL_ERROR("device->CreateRenderTargetView failed");
	}

	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	depthBufferDesc.Width = desc.width;
	depthBufferDesc.Height = desc.height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = rhiFormatToApi(desc.depthFormat);
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	result = device->CreateTexture2D(&depthBufferDesc, nullptr, &sc.depthStencilBuffer);
	if (FAILED(result))
	{
		AES_FATAL_ERROR("device depthStencilBuffer failed");
	}

	// Initialize the description of the stencil state.
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
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
	ID3D11DepthStencilState* depthStencilState;
	result = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
	if (FAILED(result))
	{
		AES_FATAL_ERROR("device->CreateDepthStencilState failed");
	}

	deviceContext->OMSetDepthStencilState(depthStencilState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = rhiFormatToApi(desc.depthFormat);
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	result = device->CreateDepthStencilView(sc.depthStencilBuffer, &depthStencilViewDesc, &sc.depthStencilView);
	if (FAILED(result))
	{
		AES_FATAL_ERROR("device->CreateDepthStencilView failed");
	}

	// where should I put this ?
	D3D11_VIEWPORT viewport;
	viewport.Width = static_cast<float>(desc.width);
	viewport.Height = static_cast<float>(desc.height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	deviceContext->RSSetViewports(1, &viewport);

	return { std::move(sc) };
}

Result<RHIRenderTarget> D3D11Device::createRenderTarget(RenderTargetDescription const& desc)
{
	AES_PROFILE_FUNCTION();

	RHIRenderTarget rt;

	D3D11_TEXTURE2D_DESC textureDesc;

	///////////////////////// Map's Texture
	// Initialize the  texture description.
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	// Setup the texture description.
	textureDesc.Width = desc.width;
	textureDesc.Height = desc.height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = rhiFormatToApi(desc.format);
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	/////////////////////// Map's Render Target
	// Setup the description of the render target view.
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	// Create the texture
	auto result = device->CreateTexture2D(&textureDesc, NULL, &rt.renderTargetTexture);
	if (FAILED(result))
	{
		AES_FATAL_ERROR("device->CreateTexture2D failed");
	}

	result = device->CreateRenderTargetView(rt.renderTargetTexture, &renderTargetViewDesc, &rt.renderTargetView);
	if (FAILED(result))
	{
		AES_FATAL_ERROR("device->CreateRenderTargetView failed");
	}

	return {std::move(rt)};
}

Result<RHIBuffer> D3D11Device::createBuffer(BufferDescription const& desc)
{
	AES_PROFILE_FUNCTION();

	validateBufferDescription(desc);
	//AES_ASSERT(desc.sizeInBytes % 16 == 0); // D3D11 buffers size must be a multiple of 16
	RHIBuffer buffer;

	buffer.size = desc.sizeInBytes;

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.Usage = rhiMemoryUsageToApi(desc.usage);
	bufferDesc.ByteWidth = buffer.size;
	bufferDesc.BindFlags = rhiBufferBindFlagsToApi(desc.bindFlags);
	bufferDesc.CPUAccessFlags = rhiCPUAccessFlagsToApi(desc.cpuAccessFlags);
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	HRESULT result;
	if (desc.initialData)
	{
		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = desc.initialData;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;
		result = device->CreateBuffer(&bufferDesc, &vertexData, &buffer.apiBuffer);
	}
	else
	{
		result = device->CreateBuffer(&bufferDesc, nullptr, &buffer.apiBuffer);
	}

	if (FAILED(result))
	{
		AES_LOG_ERROR("Failed to create GPU buffer");
		return { AESError::GPUBufferCreationFailed };
	}

	return { std::move(buffer) };
}

Result<void> aes::D3D11Device::copyBuffer(RHIBuffer const& from, RHIBuffer& to)
{
	AES_PROFILE_FUNCTION();

	deviceContext->CopyResource(to.apiBuffer, from.apiBuffer);
	return {};
}

Result<RHITexture> aes::D3D11Device::createTexture(TextureDescription const& info)
{
	AES_PROFILE_FUNCTION();

	validateTextureDescription(info);
	
	RHITexture tex;

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Height = info.height;
	textureDesc.Width = info.width;
	textureDesc.MipLevels = 1; // info.mipsLevel;
	textureDesc.ArraySize = 1;
	textureDesc.Format = rhiFormatToApi(info.format);
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = rhiMemoryUsageToApi(info.usage);
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;  // @Review
	textureDesc.CPUAccessFlags = rhiCPUAccessFlagsToApi(info.cpuAccess);
	//textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	HRESULT err;
	if (info.initialData)
	{
		D3D11_SUBRESOURCE_DATA subresource;
		subresource.pSysMem = info.initialData;
		subresource.SysMemPitch = getFormatSize(info.format) * info.width;
		err = device->CreateTexture2D(&textureDesc, &subresource, &tex.texture);
	}
	else
	{
		err = device->CreateTexture2D(&textureDesc, nullptr, &tex.texture);
	}

	if (FAILED(err))
	{
		AES_LOG_ERROR("D3D11 CreateTexture2D failed, err: {}", err);
		return { AESError::GPUTextureCreationFailed };
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	// Setup the shader resource view description.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1; // @Review

	// Create the shader resource view for the texture.
	err = device->CreateShaderResourceView(tex.texture, &srvDesc, &tex.textureView);
	if (FAILED(err))
	{
		AES_LOG_ERROR("D3D11 CreateShaderResourceView failed !");
		return { AESError::GPUTextureCreationFailed };
	}
	//D3D11Renderer::instance().getDeviceContext()->GenerateMips(textureView);

	return {std::move(tex)};
}

Result<RHIVertexShader> aes::D3D11Device::createVertexShader(VertexShaderDescription const& desc)
{
	AES_PROFILE_FUNCTION();

	RHIVertexShader vert;

	ID3DBlob* errorMessage = nullptr;
	ID3DBlob* vertexShaderBuffer = nullptr;

	if (!std::holds_alternative<aes::String>(desc.source))
	{
		AES_NOT_IMPLEMENTED();
	}
	auto const& source = std::get<aes::String>(desc.source);
	auto result = D3DCompile(source.data(), sizeof(char) * source.size(), "vertexShader", nullptr, nullptr, "main", "vs_5_0", 0, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to compile vertex shader : {}", (char*)errorMessage->GetBufferPointer());
		return { AESError::ShaderCompilationFailed };
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &vert.vertexShader);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to create vertex shader");
		return { AESError::ShaderCreationFailed };
	}

	D3DReflect(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&vert.reflector);
	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VertexType stucture in the shader.
	Array<D3D11_INPUT_ELEMENT_DESC> polygonLayout;
	polygonLayout.resize(desc.verticesLayout.size());
	for (int i = 0; auto const& layout : desc.verticesLayout)
	{
		polygonLayout[i].SemanticName = getSemanticName(layout.semantic);
		polygonLayout[i].SemanticIndex = 0;
		polygonLayout[i].Format = rhiFormatToApi(layout.format);
		polygonLayout[i].InputSlot = 0;
		polygonLayout[i].AlignedByteOffset = layout.offset;
		polygonLayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[i].InstanceDataStepRate = 0;
		i++;
	}

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout.data(), std::size(polygonLayout), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &vert.layout);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to create InputLayout");
		return { AESError::ShaderCreationFailed };
	}

	vertexShaderBuffer->Release();

	return {std::move(vert)};
}

Result<RHIFragmentShader> D3D11Device::createFragmentShader(FragmentShaderDescription const& desc)
{
	AES_PROFILE_FUNCTION();

	RHIFragmentShader frag;

	ID3DBlob* errorMessage = nullptr;
	ID3DBlob* pixelShaderBuffer = nullptr;

	if (!std::holds_alternative<aes::String>(desc.source))
	{
		AES_NOT_IMPLEMENTED();
	}

	auto const& source = std::get<aes::String>(desc.source);
	HRESULT result = D3DCompile(source.data(), sizeof(char) * source.size(), "pixelShader", nullptr, nullptr, "main", "ps_5_0", 0, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to compile pixed shader : {}", (char*)errorMessage->GetBufferPointer());
		return { AESError::ShaderCompilationFailed };
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &frag.pixelShader);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to create pixel shader");
		return { AESError::ShaderCreationFailed };
	}

	result = D3DReflect(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&frag.reflector);
	pixelShaderBuffer->Release();
	if (FAILED(result))
	{
		AES_LOG_ERROR("D3DReflect error failed to reflect shader");
	}

	if (desc.blendInfo)
	{
		auto resultBlend = createBlendState(*desc.blendInfo);
		if (!resultBlend)
			return { resultBlend.error() };

		frag.blendState = std::move(resultBlend.value());
	}

	return {std::move(frag)};
}

Result<RHISampler> aes::D3D11Device::createSampler(SamplerDescription const& desc)
{
	AES_PROFILE_FUNCTION();
	RHISampler sampler;

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = rhiTextureFilterToApi(desc.filter);
	samplerDesc.AddressU = rhiTextureAddressModeToApi(desc.addressU);
	samplerDesc.AddressV = rhiTextureAddressModeToApi(desc.addressV);
	samplerDesc.AddressW = rhiTextureAddressModeToApi(desc.addressU); // @Review
	samplerDesc.MipLODBias = desc.lodBias; // @Review 
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = desc.lodMin; // @Review 
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	auto result = device->CreateSamplerState(&samplerDesc, &sampler.samplerState);
	if (FAILED(result))
		return { AESError::SamplerCreationFailed };

	return {std::move(sampler)};
}

Result<void*> aes::D3D11Device::mapBuffer(RHIBuffer const& buffer)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	auto const result = deviceContext->Map(buffer.apiBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return { AESError::GPUBufferMappingFailed };
	}
	return { mappedResource.pData };
}

Result<void> aes::D3D11Device::unmapBuffer(RHIBuffer const& buffer)
{
	deviceContext->Unmap(buffer.apiBuffer, 0);
	return {};
}

Result<D3D11BlendState> D3D11Device::createBlendState(BlendInfo const& info)
{
	AES_PROFILE_FUNCTION();
	D3D11BlendState blend;

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = rhiBlendFactorToApi(info.colorSrc);
	blendDesc.RenderTarget[0].DestBlend = rhiBlendFactorToApi(info.colorDst);
	blendDesc.RenderTarget[0].BlendOp = rhiBlendOpToApi(info.colorOp);
	blendDesc.RenderTarget[0].SrcBlendAlpha = rhiBlendFactorToApi(info.alphaSrc);
	blendDesc.RenderTarget[0].DestBlendAlpha = rhiBlendFactorToApi(info.alphaDst);
	blendDesc.RenderTarget[0].BlendOpAlpha = rhiBlendOpToApi(info.alphaOp);
	// TODO
	blendDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	auto err = device->CreateBlendState(&blendDesc, &blend.blendState);
	if (FAILED(err))
	{
		AES_LOG_ERROR("failed to create D3D11 Blendstate");
		return { AESError::BlendStateCreationFailed };
	}

	return {std::move(blend)};
}

void D3D11Device::clearRenderTarget(RHIRenderTarget& rt)
{
	AES_PROFILE_FUNCTION();

	float color[4];
	color[0] = 0.0f;
	color[1] = 0.0f;
	color[2] = 0.0f;
	color[3] = 1.0f;

	deviceContext->ClearRenderTargetView(rt.renderTargetView, color);
}

void D3D11Device::clearSwapchain(RHISwapchain& swp)
{
	float color[4];

	color[0] = 0.0f;
	color[1] = 0.0f;
	color[2] = 0.0f;
	color[3] = 1.0f;

	deviceContext->ClearRenderTargetView(swp.rtview, color);
	deviceContext->ClearDepthStencilView(swp.depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void D3D11Device::swapBuffers(RHISwapchain const& sc)
{
	AES_PROFILE_FUNCTION();
	sc.swapchain->Present(0, 0);
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
		device = nullptr;
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

void D3D11Device::beginRenderPass(RHIRenderTarget& rt)
{
	//deviceContext->OMSetRenderTargets(1, &rt.renderTargetView, );
}

void D3D11Device::beginRenderPass(RHISwapchain& sc)
{
	deviceContext->OMSetRenderTargets(1, &sc.rtview, sc.depthStencilView);
}

void D3D11Device::endRenderPass()
{
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
	AES_PROFILE_FUNCTION();
	rasterStateDesc.AntialiasedLineEnable = false;
	rasterStateDesc.DepthBias = 0;
	rasterStateDesc.DepthBiasClamp = 0.0f;
	rasterStateDesc.DepthClipEnable = true;
	rasterStateDesc.FillMode = D3D11_FILL_SOLID;
	rasterStateDesc.FrontCounterClockwise = false;
	rasterStateDesc.MultisampleEnable = false;
	rasterStateDesc.ScissorEnable = false;
	rasterStateDesc.SlopeScaledDepthBias = 0.0f;
	HRESULT const result = device->CreateRasterizerState(&rasterStateDesc, &rasterState);
	if (FAILED(result))
	{
		AES_FATAL_ERROR("Failed to create RasterizerState !");
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

Result<void> D3D11Device::bindVertexUniformBuffer(RHIBuffer& buffer, uint slot)
{
	AES_PROFILE_FUNCTION();
	deviceContext->VSSetConstantBuffers(slot, 1, &buffer.apiBuffer);
	return{};
}
