#include "RHIElements.hpp"
#include "core/debug.hpp"

void wob::validateVertexShaderDescription(VertexShaderDescription const& desc)
{
	WOB_ASSERT(!desc.verticesLayout.empty());

	for (auto it1 = desc.verticesLayout.begin(); it1 != desc.verticesLayout.end(); ++it1)
	{
		for (auto it2 = it1 + 1; it2 != desc.verticesLayout.end(); ++it2)
		{
			WOB_ASSERT(it1->parameterName != it2->parameterName);
			WOB_ASSERT(it1->offset != it2->offset);
		}
	}
}

void wob::validateTextureDescription(TextureDescription const& desc)
{
	// @Review check if power of 2 ?
}

void wob::validateBufferDescription(BufferDescription const& desc)
{
}
