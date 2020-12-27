#include "FontManager.hpp"
#include <stb/stb_truetype.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#include <vector>
#include <algorithm>

#include "core/debug.hpp"
#include "core/utility.hpp"
#include "renderer/D3D11renderer.hpp"
#include "core/color.hpp"

#include <d3dcompiler.h>

const char pxShader[] = R"(
struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : UV;
};

Texture2D shaderTexture;
SamplerState SampleType;

float4 main(VS_OUTPUT input) : SV_TARGET
{
    return shaderTexture.Sample(SampleType, input.texCoord);
	//return float4(input.texCoord.x, input.texCoord.y, 0.0f, 1.0f);
}
)";

const char vShader[] = R"(
struct VS_INPUT
{
    float2 position : POSITION;
    float2 texCoord : UV;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : UV;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = float4(input.position.x, input.position.y, 0.0f, 1.0f);
    output.texCoord = input.texCoord;
    return output;
})";


using namespace aes;

// @Redo
static const char* getAsciiStr()
{
	static char asciiTable[256];
	for (int i = 0; i < std::size(asciiTable); i++)
		asciiTable[i] = static_cast<char>(i);
	return asciiTable;
}

// https://stackoverflow.com/a/33085495
static std::vector<uint32_t> createCheckboard(uint resX, uint resY, float segmentSizef = 0.1f)
{
	uint const segmentSizeX = std::round(resX * segmentSizef);
	uint const segmentSizeY = std::round(resY * segmentSizef);
	
	std::vector<uint32_t> bitmap(resX * resY);
	for (uint i = 0; i < resX; i++)
	{
		int const segmentNumI = i / segmentSizeX;
		for (uint j = 0; j < resY; j++)
		{
			int const segmentNumJ = j / segmentSizeY;

			bitmap[i + j * resX] = segmentNumI % 2 != segmentNumJ % 2 ? Color::BLUE : Color::RED;
		}
	}
	return bitmap;
}

// https://github.com/justinmeiners/stb-truetype-example/blob/master/main.c
// https://hero.handmade.network/episode/code/day163/
Result<void> FontManager::init()
{
	AES_PROFILE_FUNCTION();
	
	stbtt_fontinfo info;
	std::vector<uchar> courierFontBuffer = readFileBin("assets/fonts/courier.ttf");
	if (stbtt_InitFont(&info, courierFontBuffer.data(), 0) == 0)
	{
		AES_LOG_ERROR("failed to init default font");
		return { AESError::FontInitFailed };
	}

	int width, height, xOffset, yOffset;
	uint8_t* data = stbtt_GetCodepointBitmap(&info, 0, stbtt_ScaleForPixelHeight(&info, 500.f), 'N', &width, &height, &xOffset, &yOffset);
	std::vector<uint32_t> bitmap(width * height);
	for (int i = 0; i < bitmap.size(); i++)
	{
		uint8_t alpha = data[i];
		bitmap[i] = (alpha << 24) | (alpha << 16) | (alpha << 8) | (alpha << 0);
	}
	stbtt_FreeBitmap(data, nullptr);
	//auto bitmap = createCheckboard(width, height);
	//stbi_write_png("out.png", width, height, 1, bitmap.data(), width);

	//uint const b_w = 512; /* bitmap width */
	//uint const b_h = 128; /* bitmap height */
	//uint const l_h = 64;  /* line height */

	//std::vector<uchar> bitmap(b_w * b_h);

	//float const scale = stbtt_ScaleForPixelHeight(&info, l_h);

	//const char* alphabet = "hello"; getAsciiStr();

	//int ascent, descent, lineGap;
	//stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
	//ascent = roundf(ascent * scale);
	//descent = roundf(descent * scale);

	//int x = 0;
	//for (int i = 0; i < 5; ++i)
	//{
	//	/* how wide is this character */
	//	int ax;
	//	int lsb;
	//	stbtt_GetCodepointHMetrics(&info, alphabet[i], &ax, &lsb);

	//	/* get bounding box for character (may be offset to account for chars that dip above or below the line */
	//	int c_x1, c_y1, c_x2, c_y2;
	//	stbtt_GetCodepointBitmapBox(&info, alphabet[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);

	//	/* compute y (different characters have different heights */
	//	int const y = ascent + c_y1;

	//	/* render character (stride and offset is important here) */
	//	int const byteOffset = x + roundf(lsb * scale) + (y * b_w);
	//	stbtt_MakeCodepointBitmap(&info, bitmap.data() + byteOffset, c_x2 - c_x1, c_y2 - c_y1, b_w, scale, scale, alphabet[i]);

	//	/* advance x */
	//	x += roundf(ax * scale);

	//	/* add kerning */
	//	int kern = stbtt_GetCodepointKernAdvance(&info, alphabet[i], alphabet[i + 1]);
	//	x += roundf(kern * scale);
	//}
	//stbi_write_png("out.png", width, height, 4, bitmap.data(), width * 4);
	
	ID3D11Device* device = D3D11Renderer::Instance().getDevice();

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DYNAMIC;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// @TODO
	D3D11_SUBRESOURCE_DATA subresource;
	subresource.pSysMem = bitmap.data();
	subresource.SysMemPitch = textureDesc.Width * sizeof(uint32_t);
	subresource.SysMemSlicePitch = 0;

	HRESULT result = device->CreateTexture2D(&textureDesc, &subresource, &defaultFont.texture);
	if (FAILED(result))
	{
		AES_ERROR("Failed to create font texture");
		return { AESError::GPUTextureCreationFailed };
	}
	ID3D11DeviceContext* ctx = D3D11Renderer::Instance().getDeviceContext();

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	
	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &defaultFont.sampleState);
	if (FAILED(result))
	{
		return { AESError::Undefined }; // @TODO
	}
	
	D3D11_SHADER_RESOURCE_VIEW_DESC resourceViewDesc = {};
	resourceViewDesc.Format = textureDesc.Format;
	resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resourceViewDesc.Texture2D.MostDetailedMip = 0;
	resourceViewDesc.Texture2D.MipLevels = -1;
	
	result = device->CreateShaderResourceView(defaultFont.texture, &resourceViewDesc, &defaultFont.textureView);
	if (FAILED(result))
	{
		AES_ERROR("Failed to create CreateShaderResourceView");
		return { AESError::Undefined };
	}
	
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 250; // @TODO
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&vertexBufferDesc, nullptr, &vertexBuffer);
	if (FAILED(result))
	{
		AES_ERROR("Failed to create font vertex buffer");
		return { AESError::GPUBufferCreationFailed };
	}

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	indexBufferDesc.ByteWidth = sizeof(uint32_t) * 300; // @TODO
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&indexBufferDesc, nullptr, &indexBuffer);
	if (FAILED(result))
	{
		AES_ERROR("Failed to create font index buffer");
		return { AESError::GPUBufferCreationFailed };
	}
	
	ID3D10Blob* errorMessage = nullptr;
	ID3D10Blob* vertexShaderBuffer = nullptr;
	ID3D10Blob* pixelShaderBuffer = nullptr;

	// Compile the vertex shader code.
	result = D3DCompile(pxShader, sizeof(pxShader), "pixelShader", nullptr, nullptr, "main", "ps_5_0", 0, 0, &pixelShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		AES_ERROR("failed to compile pixed shader : {}", (char*)errorMessage->GetBufferPointer());
		return { AESError::ShaderCompilationFailed };
	}
	result = D3DCompile(vShader, sizeof(vShader), "vertexShader", nullptr, nullptr, "main", "vs_5_0", 0, 0, &vertexShaderBuffer, &errorMessage);
	if (FAILED(result))
	{
		AES_ERROR("failed to compile vertex shader : {}", (char*)errorMessage->GetBufferPointer());
		return { AESError::ShaderCompilationFailed };
	}

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), nullptr, &vertexShader);
	if (FAILED(result))
	{
		AES_ERROR("failed to create vertex shader");
		return { AESError::ShaderCreationFailed };
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), nullptr, &pixelShader);
	if (FAILED(result))
	{
		AES_ERROR("failed to create pixel shader");
		return { AESError::ShaderCreationFailed };
	}

	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "UV";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	result = device->CreateInputLayout(polygonLayout, std::size(polygonLayout), vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &layout);
	if (FAILED(result))
	{
		AES_ERROR("failed to create InputLayout");
		return { AESError::Undefined }; // 
	}

	vertexShaderBuffer->Release();
	pixelShaderBuffer->Release();

	return {};
}

// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-11-2d-text/
void FontManager::drawString(GraphicString const& gstring)
{
	AES_PROFILE_FUNCTION();

	float const size = gstring.textSize;
	for (uint i = 0; i < gstring.str.size(); i++)
	{
		glm::vec2 const vertex_up_left = glm::vec2(gstring.pos.x + i * size, gstring.pos.y + size);
		glm::vec2 const vertex_up_right = glm::vec2(gstring.pos.x + i * size + size, gstring.pos.y + size);
		glm::vec2 const vertex_down_right = glm::vec2(gstring.pos.x + i * size + size, gstring.pos.y);
		glm::vec2 const vertex_down_left = glm::vec2(gstring.pos.x + i * size, gstring.pos.y);

		char const character = gstring.str[i];
		float const uv_x = (character % 16) / 16.0f;
		float const uv_y = (character / 16) / 16.0f;

		//glm::vec2 const uv_up_left = glm::vec2(uv_x, 1.0f - uv_y);
		//glm::vec2 const uv_up_right = glm::vec2(uv_x + 1.0f / 16.0f, 1.0f - uv_y);
		//glm::vec2 const uv_down_right = glm::vec2(uv_x + 1.0f / 16.0f, 1.0f - (uv_y + 1.0f / 16.0f));
		//glm::vec2 const uv_down_left = glm::vec2(uv_x, 1.0f - (uv_y + 1.0f / 16.0f));

		glm::vec2 const uv_up_left = glm::vec2(0.0f, 0.0f);
		glm::vec2 const uv_up_right = glm::vec2(1.0f, 0.0f);
		glm::vec2 const uv_down_right = glm::vec2(1.0f, 1.0f);
		glm::vec2 const uv_down_left = glm::vec2(0.0f, 1.0f);

		vertices.push_back(Vertex{ vertex_up_left, uv_up_left });
		vertices.push_back(Vertex{ vertex_down_left, uv_down_left });
		vertices.push_back(Vertex{ vertex_up_right, uv_up_right });
		vertices.push_back(Vertex{ vertex_down_right, uv_down_right });

		indices.push_back(i + 0);
		indices.push_back(i + 2);
		indices.push_back(i + 1);
		indices.push_back(i + 3);
	}
}

void FontManager::draw()
{
	AES_PROFILE_FUNCTION();
	ID3D11DeviceContext* ctx = D3D11Renderer::Instance().getDeviceContext();

	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ctx->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		Vertex* dataPtr = (Vertex*)mappedResource.pData;
		memcpy(dataPtr, vertices.data(), vertices.size() * sizeof(Vertex));

		ctx->Unmap(vertexBuffer, 0);
	}
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ctx->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

		uint32_t* dataPtr = (uint32_t*)mappedResource.pData;
		memcpy(dataPtr, indices.data(), indices.size() * sizeof(uint32_t));

		ctx->Unmap(indexBuffer, 0);
	}

	ctx->IASetInputLayout(layout);
	ctx->PSSetSamplers(0, 1, &defaultFont.sampleState);
	ctx->PSSetShaderResources(0, 1, &defaultFont.textureView);
	ctx->VSSetShader(vertexShader, nullptr, 0);
	ctx->PSSetShader(pixelShader, nullptr, 0);

	uint stride = sizeof(Vertex);
	uint offset = 0;
	ctx->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	ctx->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	ctx->DrawIndexed(indices.size(), 0, 0);
	vertices.clear();
	indices.clear();
}

void FontManager::destroy()
{
	AES_PROFILE_FUNCTION();

	vertexShader->Release();
	pixelShader->Release();
	vertexBuffer->Release();
	indexBuffer->Release();
	layout->Release();

	// @TODO Release fonts
}


