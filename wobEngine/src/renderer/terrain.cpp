#include "terrain.hpp"
#include "qoi/qoi.h"

using namespace wob;

void Terrain::init()
{
	qoi_desc desc;
	void* imageData = qoi_read(R"(C:\Users\grego\source\repos\AESengine\wobEngine\assets\textures\heightmap.qoi)", &desc, 0);
	TextureDescription textureDesc;
	textureDesc.cpuAccess = CPUAccessFlagBits::Write;
	textureDesc.width = desc.width;
	textureDesc.height = desc.height;
	textureDesc.format = RHIFormat::R8G8B8A8_Uint;
	textureDesc.usage = MemoryUsage::Immutable;
	textureDesc.initialData = imageData;
}
