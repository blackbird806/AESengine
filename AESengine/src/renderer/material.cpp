#include "material.hpp"
#include "RHI/RHIRenderContext.hpp"

using namespace aes;

Result<void> Material::init(RHIVertexShader* vs, RHIFragmentShader* fs)
{
	AES_ASSERT(vs);
	AES_ASSERT(fs);
	
	vertexShader = vs;
	fragmentShader = fs;
	
	auto vsbufferInfos = vertexShader->getUniformBufferInfos();
	for (auto const& bufferInfo : vsbufferInfos)
	{
		vsUniformBufferSlots[bufferInfo.name] = bufferInfo.index;
	}

	auto fsbufferInfos = fragmentShader->getUniformBufferInfos();
	for (auto const& bufferInfo : fsbufferInfos)
	{
		fsUniformBufferSlots[bufferInfo.name] = bufferInfo.index;
	}

	return {};
}

void Material::bind(BindInfo const& bindInfos)
{
	auto context = RHIRenderContext::instance();
	
	context.setVertexShader(*vertexShader);
	for (auto const& [bufferName, buffer] : bindInfos.vsBuffers)
		context.bindVSUniformBuffer(*buffer, vsUniformBufferSlots[bufferName]);
	
	context.setFragmentShader(*fragmentShader);
	for (auto const& [bufferName, buffer] : bindInfos.fsBuffers)
		context.bindFSUniformBuffer(*buffer, fsUniformBufferSlots[bufferName]);
}
