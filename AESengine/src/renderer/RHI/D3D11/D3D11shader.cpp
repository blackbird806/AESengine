#include "D3D11shader.hpp"

#include <d3dcompiler.h>

#include "core/aes.hpp"
#include "core/debug.hpp"
#include "D3D11renderer.hpp"
#include "renderer/vertex.hpp"

using namespace aes;

const char pxShader[] = R"(
struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
    return input.color;
}
)";

const char vShader[] = R"(
struct VS_INPUT
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer CameraBuffer
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};

cbuffer ModelBuffer
{
	float4x4 worldMatrix;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    //output.position = input.position;

    // Store the input color for the pixel shader to use.
    output.color = input.color;
    
    return output;
})";

void D3D11Shader::init()
{
	init(vShader, pxShader);
}

void D3D11Shader::init(std::string_view vs, std::string_view ps)
{
	AES_PROFILE_FUNCTION();

	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;

	ID3D11Device* device = D3D11Renderer::instance().getDevice();

	// Compile the vertex shader code.
	HRESULT result = D3DCompile(ps.data(), sizeof(char) * ps.size(), "pixelShader", nullptr, nullptr, "main", "ps_5_0", 0, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to compile pixed shader : {}", (char*)errorMessage->GetBufferPointer());
		return;
	}
	result = D3DCompile(vs.data(), sizeof(char) * vs.size(), "vertexShader", nullptr, nullptr, "main", "vs_5_0", 0, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to compile vertex shader : {}", (char*)errorMessage->GetBufferPointer());
		return;
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertexShader);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to create vertex shader");
		return;
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixelShader);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to create pixel shader");
		return;
	}

	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	uint numElements = std::size(polygonLayout);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &layout);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to create InputLayout");
		return;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	pixelShaderBuffer->Release();

	D3D11_BUFFER_DESC cameraBufferDesc = {};
	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	cameraBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufferDesc.ByteWidth = sizeof(ModelBuffer);
	cameraBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufferDesc.MiscFlags = 0;
	cameraBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&cameraBufferDesc, NULL, &cameraBuffer);
	if (FAILED(result))
	{
		AES_ERROR("failed to create Model buffer");
	}
}

void D3D11Shader::destroy()
{
	AES_PROFILE_FUNCTION();

	AES_ASSERT(layout != nullptr);
	AES_ASSERT(pixelShader != nullptr);
	AES_ASSERT(vertexShader != nullptr);
	AES_ASSERT(cameraBuffer != nullptr);

	layout->Release();
	pixelShader->Release();
	vertexShader->Release();
	cameraBuffer->Release();
}

void D3D11Shader::render(glm::mat4 const& view, glm::mat4 const& proj)
{
	AES_PROFILE_FUNCTION();

	ID3D11DeviceContext* deviceContext = D3D11Renderer::instance().getDeviceContext();

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	// Set the vertex input layout.
	deviceContext->IASetInputLayout(layout);

	auto result = deviceContext->Map(cameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		AES_ERROR("failed to map UBO");
	}
	// Get a pointer to the data in the constant buffer.
	CameraBuffer* dataPtr = static_cast<CameraBuffer*>(mappedResource.pData);

	// Copy the matrices into the constant buffer.
	dataPtr->view = glm::transpose(view);
	dataPtr->proj = glm::transpose(proj);

	// Unlock the constant buffer.
	deviceContext->Unmap(cameraBuffer, 0);

	deviceContext->VSSetConstantBuffers(0, 1, &cameraBuffer);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(vertexShader, nullptr, 0);
	deviceContext->PSSetShader(pixelShader, nullptr, 0);
}
