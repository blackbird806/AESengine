#ifndef AES_D3D11SHADER_HPP
#define AES_D3D11SHADER_HPP

#include <d3d11.h>
#include <d3d11shader.h>
#include "core/error.hpp"
#include "renderer/RHI/RHIShaderReflection.hpp"
#include "D3D11BlendState.hpp"

namespace aes {
	
	struct FragmentShaderDescription;
	struct VertexShaderDescription;

	// @Review d3d11 shader architecture
	class D3D11Shader
	{
	public:
		D3D11Shader() = default;
		D3D11Shader(D3D11Shader&& rhs) noexcept;
		D3D11Shader& operator=(D3D11Shader&& rhs) noexcept;
		//std::vector<UniformBufferReflectionInfo> getUniformBufferInfos() const;
		virtual void destroy();
		virtual ~D3D11Shader() noexcept;

	protected:
		ID3D11ShaderReflection* reflector = nullptr;
	};
	
	class D3D11VertexShader : public D3D11Shader
	{
		friend class D3D11Device;
	public:
		D3D11VertexShader() = default;
		D3D11VertexShader(D3D11VertexShader&& rhs) noexcept;
		D3D11VertexShader(D3D11VertexShader const& rhs) = delete;
		D3D11VertexShader& operator=(D3D11VertexShader&&) noexcept;
		D3D11VertexShader& operator=(D3D11VertexShader const&) = delete;
		virtual void destroy() override;

		ID3D11InputLayout* getInputLayout();
		ID3D11VertexShader* getHandle();
		
	private:
	
		ID3D11VertexShader* vertexShader = nullptr;
		ID3D11InputLayout* layout = nullptr;
	};

	class D3D11FragmentShader : public D3D11Shader
	{
		friend class D3D11Device;
	public:
		
		D3D11FragmentShader() = default;
		D3D11FragmentShader(D3D11FragmentShader&& rhs) noexcept;
		D3D11FragmentShader(D3D11FragmentShader const& rhs) = delete;
		D3D11FragmentShader& operator=(D3D11FragmentShader&& rhs) noexcept;
		D3D11FragmentShader& operator=(D3D11FragmentShader const& rhs) = delete;
		virtual void destroy() override;

		ID3D11PixelShader* getHandle();
		D3D11BlendState blendState;

	private:
		ID3D11PixelShader* pixelShader = nullptr;
	};

	using RHIVertexShader = D3D11VertexShader;
	using RHIFragmentShader = D3D11FragmentShader;
}

#endif
