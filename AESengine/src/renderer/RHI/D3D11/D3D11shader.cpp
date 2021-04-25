#include "D3D11Shader.hpp"

#include <d3dcompiler.h>

#include "D3D11Elements.hpp"
#include "core/aes.hpp"
#include "core/debug.hpp"
#include "D3D11renderer.hpp"
#include "renderer/vertex.hpp"

using namespace aes;

Result<void> D3D11VertexShader::init(VertexShaderDescription const& desc)
{
	AES_PROFILE_FUNCTION();

	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;

	ID3D11Device* device = D3D11Renderer::instance().getDevice();

	if (!std::holds_alternative<std::string_view>(desc.source))
	{
		AES_NOT_IMPLEMENTED();
	}
	auto const source = std::get<std::string_view>(desc.source);
	auto result = D3DCompile(source.data(), sizeof(char) * source.size(), "vertexShader", nullptr, nullptr, "main", "vs_5_0", 0, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to compile vertex shader : {}", (char*)errorMessage->GetBufferPointer());
		return { AESError::ShaderCompilationFailed };
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &vertexShader);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to create vertex shader");
		return { AESError::ShaderCreationFailed };
	}
	
	D3DReflect(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflector);
	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VertexType stucture in the shader.
	std::vector<D3D11_INPUT_ELEMENT_DESC> polygonLayout(desc.verticesLayout.size());
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
	result = device->CreateInputLayout(polygonLayout.data(), std::size(polygonLayout), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &layout);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to create InputLayout");
		return { AESError::ShaderCreationFailed };
	}
	
	vertexShaderBuffer->Release();
}

D3D11VertexShader::D3D11VertexShader(D3D11VertexShader&& rhs) noexcept : D3D11Shader(std::move(rhs)),
	vertexShader(rhs.vertexShader), layout(rhs.layout)
{
	AES_PROFILE_FUNCTION();

	rhs.vertexShader = nullptr;
	rhs.layout = nullptr;
}

D3D11VertexShader& D3D11VertexShader::operator=(D3D11VertexShader&& rhs) noexcept
{
	AES_PROFILE_FUNCTION();
	D3D11Shader::operator=(std::move(rhs));

	vertexShader = rhs.vertexShader;
	layout = rhs.layout;
	rhs.vertexShader = nullptr;
	rhs.layout = nullptr;
	return *this;
}

D3D11VertexShader::~D3D11VertexShader()
{
	AES_PROFILE_FUNCTION();

	if (vertexShader) // if shader is valid so is layout and reflector
	{
		vertexShader->Release();
		layout->Release();
	}
}

ID3D11InputLayout* D3D11VertexShader::getInputLayout()
{
	return layout;
}

D3D11Shader::D3D11Shader(D3D11Shader&& rhs) noexcept : reflector(rhs.reflector)
{
	rhs.reflector = nullptr;
}

D3D11Shader& D3D11Shader::operator=(D3D11Shader&& rhs) noexcept
{
	reflector = rhs.reflector;
	rhs.reflector = nullptr;
	return *this;
}

std::vector<UniformBufferReflectionInfo> D3D11Shader::getUniformBufferInfos() const
{
	AES_PROFILE_FUNCTION();

	D3D11_SHADER_DESC shaderDesc;
	reflector->GetDesc(&shaderDesc);

	std::vector<UniformBufferReflectionInfo> bufferInfos(shaderDesc.ConstantBuffers);
	for (uint i = 0; i < shaderDesc.ConstantBuffers; i++)
	{
		ID3D11ShaderReflectionConstantBuffer* D3DbufferInfo = reflector->GetConstantBufferByIndex(i);
		D3D11_SHADER_BUFFER_DESC bufferDesc;
		D3DbufferInfo->GetDesc(&bufferDesc);
		
		bufferInfos[i].name = bufferDesc.Name;
		bufferInfos[i].size = bufferDesc.Size;
		bufferInfos[i].index = i;
	}

	return bufferInfos;
}

D3D11Shader::~D3D11Shader()
{
	if (reflector)
		reflector->Release();
}

ID3D11VertexShader* D3D11VertexShader::getHandle()
{
	return vertexShader;
}

Result<void> D3D11FragmentShader::init(FragmentShaderDescription const& desc)
{
	AES_PROFILE_FUNCTION();

	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;

	ID3D11Device* device = D3D11Renderer::instance().getDevice();
	
	if (!std::holds_alternative<std::string_view>(desc.source))
	{
		AES_NOT_IMPLEMENTED();
	}
	
	auto const source = std::get<std::string_view>(desc.source);
	HRESULT result = D3DCompile(source.data(), sizeof(char) * source.size(), "pixelShader", nullptr, nullptr, "main", "ps_5_0", 0, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to compile pixed shader : {}", (char*)errorMessage->GetBufferPointer());
		return { AESError::ShaderCompilationFailed };
	}

	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &pixelShader);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to create pixel shader");
		return { AESError::ShaderCreationFailed };
	}
	
	D3DReflect(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&reflector);
	pixelShaderBuffer->Release();
}

D3D11FragmentShader::D3D11FragmentShader(D3D11FragmentShader&& rhs) noexcept : D3D11Shader(std::move(rhs)), pixelShader(rhs.pixelShader)
{
	AES_PROFILE_FUNCTION();
	rhs.pixelShader = nullptr;
}

D3D11FragmentShader& D3D11FragmentShader::operator=(D3D11FragmentShader&& rhs) noexcept
{
	AES_PROFILE_FUNCTION();
	D3D11Shader::operator=(std::move(rhs));
	pixelShader = rhs.pixelShader;
	rhs.pixelShader = nullptr;
	return *this;
}

D3D11FragmentShader::~D3D11FragmentShader()
{
	AES_PROFILE_FUNCTION();

	if (pixelShader)
		pixelShader->Release();
}

ID3D11PixelShader* D3D11FragmentShader::getHandle()
{
	return pixelShader;
}
