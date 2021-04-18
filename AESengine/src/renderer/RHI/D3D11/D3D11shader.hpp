#ifndef D3D11SHADER_HPP
#define D3D11SHADER_HPP

#include <d3d11.h>
#include <d3d11shader.h>
#include <glm/glm.hpp>
#include "core/error.hpp"
#include "renderer/RHI/RHIShaderReflection.hpp"

namespace aes {
	
	struct FragmentShaderDescription;
	struct VertexShaderDescription;

	class D3D11ShaderLegacy
	{
	public:

		void init();
		void init(std::string_view vs, std::string_view ps);

		void destroy();

		void render(glm::mat4 const& view, glm::mat4 const& proj);

	private:
		
		ID3D11VertexShader* vertexShader;
		ID3D11PixelShader* pixelShader;
		ID3D11InputLayout* layout;
		ID3D11Buffer* cameraBuffer;
	};

	class D3D11Shader
	{
	public:
		std::vector<UniformBufferReflectionInfo> getUniformBufferInfos() const;
	protected:
		ID3D11ShaderReflection* reflector;
	};
	
	class D3D11VertexShader : public D3D11Shader
	{
	public:
		D3D11VertexShader() = default;
		Result<void> init(VertexShaderDescription const& desc);
		D3D11VertexShader(D3D11VertexShader&& rhs) noexcept;
		D3D11VertexShader(D3D11VertexShader const& rhs) = delete;
		D3D11VertexShader& operator=(D3D11VertexShader&&) noexcept;
		D3D11VertexShader& operator=(D3D11VertexShader const&) = delete;
		~D3D11VertexShader();


		ID3D11VertexShader* getHandle();
		
	private:
	
		ID3D11VertexShader* vertexShader;
		ID3D11InputLayout* layout;
	};

	class D3D11FragmentShader : public D3D11Shader
	{
	public:
		
		D3D11FragmentShader() = default;
		Result<void> init(FragmentShaderDescription const& desc);
		D3D11FragmentShader(D3D11FragmentShader&& rhs) noexcept;
		D3D11FragmentShader(D3D11FragmentShader const& rhs) = delete;
		D3D11FragmentShader& operator=(D3D11FragmentShader&& rhs) noexcept;
		D3D11FragmentShader& operator=(D3D11FragmentShader const& rhs) = delete;
		
		~D3D11FragmentShader();

		ID3D11PixelShader* getHandle();

	private:
		
		ID3D11PixelShader* pixelShader;
	};

	using RHIVertexShader = D3D11VertexShader;
	using RHIFragmentShader = D3D11FragmentShader;
}

#endif
