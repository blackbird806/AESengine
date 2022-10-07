#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <unordered_map>
#include <string>
#include "RHI/RHIShader.hpp"
#include "RHI/RHIBuffer.hpp"
#include "core/error.hpp"

// DEPRECATED materials will be redesigned when the new RHI will be advanced enough

namespace aes
{
	/*
	 * @TODO Materials rework
	 * Materials should ? own shaders and shaders parameters (buffers, textures, ...)
	 * Current bind system is not user friendly
	 * Set data per member name rather than per buffer ?
	 * Regroup buffers per update frequency eg rather than "CameraBuffer" and ModelBuffer ==> FrameBuffer updated each frame ?
	 */
	
	class Material
	{
	public:

		struct BindInfo
		{
			std::vector<std::pair<std::string, RHIBuffer*>> vsBuffers;
			std::vector<std::pair<std::string, RHIBuffer*>> fsBuffers;
		};

		Result<void> init(RHIVertexShader*, RHIFragmentShader*);
		
		void bind(BindInfo const& bindInfos);

	private:
		
		std::unordered_map<std::string, uint32_t> vsUniformBufferSlots;
		std::unordered_map<std::string, uint32_t> fsUniformBufferSlots;
		
		RHIVertexShader* vertexShader;
		RHIFragmentShader* fragmentShader;
	};

}

#endif