#ifndef WOB_GRAPHISPIPELINE_HPP
#define WOB_GRAPHISPIPELINE_HPP

#include "core/array.hpp"
#include "core/string.hpp"
#include "renderer/RHI/RHIDevice.hpp"
#include "renderer/RHI/RHIBuffer.hpp"
#include "renderer/RHI/RHIShader.hpp"

namespace wob
{
	// @TODO rework, we may want to hide the individual binding of shaders and buffers to the RHI and only bind a graphics pipeline instead
	// this should facilitate the vulkan port, simplify the user code and will be less error prone
	class GraphicsPipeline
	{
		public:
		
		void init(RHIDevice* device);

		void bind();
		
		void buildFragmentShader(FragmentShaderDescription const& desc);
		void buildVertexShader(VertexShaderDescription const& desc);

		void registerVertexUniform(String const& name, BufferDescription bufferDesc, uint bindPoint);
		void setVertexUniform(String const& name, void* data, uint32_t size);

		template<typename T>
		void setVertexUniform(String const& name, T val)
		{
			setVertexUniform(name, &val, sizeof(T));
		}

		void registerFragmentUniform(String const& name, BufferDescription bufferDesc, uint bindPoint);
		void setFragmentUniform(String const& name, void* data, uint32_t size);

		template<typename T>
		void setFragmentUniform(String const& name, T val)
		{
			setFragmentUniform(name, &val, sizeof(T));
		}

		struct UniformBindPoint
		{
			String name;
			RHIBuffer buffer;
			uint slot;
		};

		const VertexShaderDescription& getVertexShaderDesc();

	private:

		RHIDevice* device;
		
		VertexShaderDescription vertexShaderDesc;

		// let's give the pipeline ownership of it's resources, for now it's simpler 
		// maybe later update this if needed
		RHIVertexShader vertexShader;
		RHIFragmentShader fragmentShader;

		Array<UniformBindPoint> vertexUniformBuffers;
		Array<UniformBindPoint> fragmentUniformBuffers;
	};
}
 
#endif