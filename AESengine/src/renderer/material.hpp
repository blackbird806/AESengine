#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <unordered_map>
#include <string>

#include "RHI/RHIShader.hpp"
#include "RHI/RHIBuffer.hpp"
#include "core/error.hpp"

namespace aes
{
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