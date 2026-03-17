#ifndef WOB_TERRAIN_HPP
#define WOB_TERRAIN_HPP

#include "graphicsPipeline.hpp"
#include "RHI/RHIBuffer.hpp"
#include "RHI/RHITexture.hpp"

namespace wob
{
	class Terrain
	{
	public:

		void init();

	private:
		uint32_t height, width;

		GraphicsPipeline terrainPipeline;
		RHITexture heighmap;
		RHIBuffer vertexBuffer;
		RHIBuffer indexBuffer;
	};
}

#endif