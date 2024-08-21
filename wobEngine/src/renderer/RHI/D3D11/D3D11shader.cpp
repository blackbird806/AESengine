#include "D3D11Shader.hpp"

#include "D3D11Elements.hpp"
#include "core/aes.hpp"
#include "core/debug.hpp"

using namespace aes;

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

//std::vector<UniformBufferReflectionInfo> D3D11Shader::getUniformBufferInfos() const
//{
//	AES_PROFILE_FUNCTION();
//
//	D3D11_SHADER_DESC shaderDesc;
//	reflector->GetDesc(&shaderDesc);
//
//	std::vector<UniformBufferReflectionInfo> bufferInfos(shaderDesc.ConstantBuffers);
//	for (uint i = 0; i < shaderDesc.ConstantBuffers; i++)
//	{
//		ID3D11ShaderReflectionConstantBuffer* D3DbufferInfo = reflector->GetConstantBufferByIndex(i);
//		D3D11_SHADER_BUFFER_DESC bufferDesc;
//		D3DbufferInfo->GetDesc(&bufferDesc);
//
//		bufferInfos[i].name = bufferDesc.Name;
//		bufferInfos[i].size = bufferDesc.Size;
//		bufferInfos[i].index = i;
//	}
//
//	return bufferInfos;
//}

void aes::D3D11Shader::destroy()
{
	if (reflector)
		reflector->Release();
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

void aes::D3D11VertexShader::destroy()
{
	AES_PROFILE_FUNCTION();
	D3D11Shader::destroy();
	if (vertexShader) // if shader is valid so is layout and reflector
	{
		vertexShader->Release();
		layout->Release();
	}
}

ID3D11VertexShader* D3D11VertexShader::getHandle()
{
	return vertexShader;
}

ID3D11InputLayout* D3D11VertexShader::getInputLayout()
{
	return layout;
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

void aes::D3D11FragmentShader::destroy()
{
	AES_PROFILE_FUNCTION();
	D3D11Shader::destroy();

	if (pixelShader)
		pixelShader->Release();
}

ID3D11PixelShader* D3D11FragmentShader::getHandle()
{
	return pixelShader;
}
