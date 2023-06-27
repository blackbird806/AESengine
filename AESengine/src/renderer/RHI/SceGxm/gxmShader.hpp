#ifndef GXMSHADER_HPP
#define GXMSHADER_HPP

#include "core/error.hpp"
#include "renderer/RHI/RHIShaderReflection.hpp"
#include "renderer/RHI/RHIElements.hpp"
#include "core/platformVita/vitagxm.h"
#include <vector>

namespace aes
{
	class GxmShader
	{
		public:
		GxmShader() = default;
		GxmShader(GxmShader&& rhs) noexcept;
		GxmShader& operator=(GxmShader&& rhs) noexcept;
		~GxmShader();

		SceGxmProgram const* getGxpShader() const { return gxpShader; }

		std::vector<UniformBufferReflectionInfo> getUniformBufferInfos() const;

		protected:
		SceGxmProgram const* gxpShader;
		SceGxmShaderPatcherId id;
	};

	class GxmVertexShader : public GxmShader
	{
		public:
		GxmVertexShader() = default;
		GxmVertexShader(GxmVertexShader const&) = delete;
		GxmVertexShader(GxmVertexShader&&) noexcept;
		GxmVertexShader& operator=(GxmVertexShader const&) = delete;
		GxmVertexShader& operator=(GxmVertexShader&&) noexcept;
		~GxmVertexShader();

		Result<void> init(VertexShaderDescription const& desc);
		
		SceGxmVertexProgram* getHandle();

		private:
		
		SceGxmVertexProgram* vertexShader = nullptr;
	};

	class GxmFragmentShader : public GxmShader
	{
		public:
		GxmFragmentShader() = default;
		GxmFragmentShader(GxmFragmentShader const &) = delete;
		GxmFragmentShader(GxmFragmentShader&&) noexcept;
		GxmFragmentShader& operator=(GxmFragmentShader const&) = delete;
		GxmFragmentShader& operator=(GxmFragmentShader&&) noexcept;
		~GxmFragmentShader();
		
		Result<void> init(FragmentShaderDescription const& desc);

		SceGxmFragmentProgram* getHandle();
	
		private:
		
		SceGxmFragmentProgram* fragmentShader = nullptr;
	};

	using RHIFragmentShader = GxmFragmentShader;
	using RHIVertexShader = GxmVertexShader;
}

#endif
