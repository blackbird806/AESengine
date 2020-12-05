#include "Renderer2D.hpp"
#include "core/debug.hpp"
#include "D3D11renderer.hpp"

#include <d3dcompiler.h>

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
    float2 position : POSITION;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = float4(input.position.x, input.position.y, 0.0f, 1.0f);
    output.color = input.color;
    return output;
})";

using namespace aes;

Renderer2D& aes::Renderer2D::Instance()
{
	static Renderer2D instance;
	return instance;
}

void Renderer2D::init()
{
	AES_PROFILE_FUNCTION();

	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * maxVertices;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	ID3D11Device* device = D3D11Renderer::Instance().getDevice();
	HRESULT result = device->CreateBuffer(&vertexBufferDesc, nullptr, &vertexBuffer);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to create immediate vertex buffer");
		return;
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	indexBufferDesc.ByteWidth = sizeof(uint32_t) * maxIndices;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&indexBufferDesc, nullptr, &indexBuffer);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to create immediate index buffer");
		return;
	}
	
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;

	// Compile the vertex shader code.
	result = D3DCompile(pxShader, sizeof(pxShader), "pixelShader", nullptr, nullptr, "main", "ps_5_0", 0, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to compile pixed shader : {}", (char*)errorMessage->GetBufferPointer());
		return;
	}
	result = D3DCompile(vShader, sizeof(vShader), "vertexShader", nullptr, nullptr, "main", "vs_5_0", 0, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to compile vertex shader : {}", (char*)errorMessage->GetBufferPointer());
		return;
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &vertexShader);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to create vertex shader");
		return;
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &pixelShader);
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
	polygonLayout[0].Format = DXGI_FORMAT_R32G32_FLOAT;
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
	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, std::size(polygonLayout), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &layout);
	if (FAILED(result))
	{
		AES_LOG_ERROR("failed to create InputLayout");
		return;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	pixelShaderBuffer->Release();
}

void Renderer2D::drawLine(glm::vec2 p1, glm::vec2 p2, glm::vec4 const& col)
{
	AES_PROFILE_FUNCTION();

	Command cmd;
	cmd.type = Command::Type::Line;
	cmd.line = { p1, p2 };
	cmd.col = col;
	commands.push_back(cmd);
}

void Renderer2D::drawFillRect(Rect const& rect, glm::vec4 const& col)
{
	AES_PROFILE_FUNCTION();

	Command cmd;
	cmd.type = Command::Type::FillRect;
	cmd.rect = rect;
	cmd.col = col;
	commands.push_back(cmd);
}

void Renderer2D::updateBuffers()
{
	AES_PROFILE_FUNCTION();

	ID3D11DeviceContext* deviceContext = D3D11Renderer::Instance().getDeviceContext();
	D3D11_MAPPED_SUBRESOURCE vertexMappedResource;
	deviceContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &vertexMappedResource);

	D3D11_MAPPED_SUBRESOURCE indexMappedResource;
	deviceContext->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &indexMappedResource);
	
	size_t verticesOffset = 0; // offset in vertices
	size_t indicesOffset = 0; // offset in uint32_t
	for (auto const& cmd : commands)
	{
		Vertex* verticesData = static_cast<Vertex*>(vertexMappedResource.pData) + verticesOffset;
		uint32_t* indicesData = static_cast<uint32_t*>(indexMappedResource.pData) + indicesOffset;
		
		switch (cmd.type)
		{
		case Command::Type::Line:
			verticesData[0] = { cmd.line.from, cmd.col };
			verticesData[1] = { cmd.line.to, cmd.col };
			indicesData[0] = 0;
			indicesData[1] = 1;
			
			verticesOffset += 2;
			indicesOffset += 2;
			break;
		case Command::Type::FillRect:
			//RectBounds const bounds = cmd.rect.getBounds();
			//verticesData[0] = { bounds.minL, cmd.col };
			//verticesData[1] = { bounds.topL, cmd.col };
			//verticesData[1] = { bounds.topL, cmd.col };
			//verticesData[1] = { bounds.topL, cmd.col };
			break;
		default:
			AES_UNREACHABLE();
		}
	}

	deviceContext->Unmap(vertexBuffer, 0);
	deviceContext->Unmap(indexBuffer, 0);
}

void Renderer2D::destroy()
{
	AES_PROFILE_FUNCTION();

	layout->Release();
	vertexBuffer->Release();
	indexBuffer->Release();
	pixelShader->Release();
}

void Renderer2D::draw()
{
	AES_PROFILE_FUNCTION();

	updateBuffers();
	ID3D11DeviceContext* ctx = D3D11Renderer::Instance().getDeviceContext();

	uint stride = sizeof(Vertex);
	uint vertexBufferOffset = 0;
	ctx->IASetInputLayout(layout);
	ctx->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &vertexBufferOffset);
	ctx->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	
	ctx->VSSetShader(vertexShader, NULL, 0);
	ctx->PSSetShader(pixelShader, NULL, 0);
	uint indicesOffset = 0;
	uint verticesOffset = 0; 
	for (uint i = 0; i < commands.size(); i++)
	{
		switch (commands[i].type)
		{
		case Command::Type::Line:
			ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
			//ctx->Draw(2, offset); // @TODO instanciation ?
			ctx->DrawIndexed(2, indicesOffset, verticesOffset); // @TODO instanciation ?
			verticesOffset += 2;
			indicesOffset += 2;
			break;
		default:
			break;
		}
	}
	commands.clear();
}

Renderer2D::Command::Command(Command const& other) :
	type(other.type), col(other.col)
{
	AES_PROFILE_FUNCTION();

	switch (type)
	{
	case Command::Type::Line:
		line = other.line;
		break;
	case Command::Type::Rect:
		rect = other.rect;
		break;
	default:
		break;
	}
}

Renderer2D::Command& Renderer2D::Command::operator=(Command const& other)
{
	AES_PROFILE_FUNCTION();

	type = other.type;
	col = other.col;

	switch (type)
	{
	case Command::Type::Line:
		line = other.line;
		break;
	case Command::Type::Rect:
		rect = other.rect;
		break;
	default:
		break;
	}
	return *this;
}
