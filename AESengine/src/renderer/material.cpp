#include "material.hpp"
#include "RHI/RHIRenderContext.hpp"

aes::Result<void> aes::Material::init(RHIVertexShader* vs, RHIFragmentShader* fs)
{
	AES_PROFILE_FUNCTION();
	AES_ASSERT(vs);
	AES_ASSERT(fs);

	vertexShader = vs;
	fragmentShader = fs;
	
	auto const vsbufferInfos = vertexShader->getUniformBufferInfos();
	AES_LOG("buffer infos size {}", vsbufferInfos.size());
	for (auto const& bufferInfo : vsbufferInfos)
	{
		vsUniformBufferSlots[bufferInfo.name] = bufferInfo.index;
		AES_LOG("uniform buffer reflect {} - index: {} - size: {}", bufferInfo.name, bufferInfo.index, bufferInfo.size);
	}

	auto const fsbufferInfos = fragmentShader->getUniformBufferInfos();
	for (auto const& bufferInfo : fsbufferInfos)
	{
		fsUniformBufferSlots[bufferInfo.name] = bufferInfo.index;
	}
	
	return {};
}

void aes::Material::bind(BindInfo const& bindInfos)
{
	AES_PROFILE_FUNCTION();

	auto& context = RHIRenderContext::instance();
	context.setVertexShader(*vertexShader);
	for (auto const& [bufferName, buffer] : bindInfos.vsBuffers)
		context.bindVSUniformBuffer(*buffer, vsUniformBufferSlots[bufferName]);

	context.setFragmentShader(*fragmentShader);
	for (auto const& [bufferName, buffer] : bindInfos.fsBuffers)
		context.bindFSUniformBuffer(*buffer, fsUniformBufferSlots[bufferName]);
}
