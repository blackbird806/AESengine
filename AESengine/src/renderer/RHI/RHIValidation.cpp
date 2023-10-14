#include "RHIElements.hpp"

void aes::validateVertexShaderDescription(VertexShaderDescription const& desc)
{
	AES_ASSERT(!desc.verticesLayout.empty());

	for (auto it1 = desc.verticesLayout.begin(); it1 != desc.verticesLayout.end(); ++it1)
	{
		for (auto it2 = it1 + 1; it2 != desc.verticesLayout.end(); ++it2)
		{
			AES_ASSERT(it1->parameterName != it2->parameterName);
			AES_ASSERT(it1->offset != it2->offset);
		}
	}
}

void aes::validateTextureDescription(TextureDescription const& desc)
{
	// @Review check if power of 2 ?
}

void aes::validateBufferDescription(BufferDescription const& desc)
{
}
