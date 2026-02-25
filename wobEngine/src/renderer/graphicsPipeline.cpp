#include "graphicsPipeline.hpp"
#include "core/string.hpp"
#include "RHI/RHIElements.hpp"
#include <ranges>
#include <algorithm>

using namespace aes;

void GraphicsPipeline::init(RHIDevice* device_)
{
	device = device_;
}

void GraphicsPipeline::bind()
{
	device->setVertexShader(vertexShader);
	device->setFragmentShader(fragmentShader);

	for (auto& uniform : vertexUniformBuffers)
	{
		device->bindVertexUniformBuffer(uniform.buffer, uniform.slot);
	}

	for (auto& uniform : fragmentUniformBuffers)
	{
		device->bindVertexUniformBuffer(uniform.buffer, uniform.slot);
	}
}

void GraphicsPipeline::buildFragmentShader(FragmentShaderDescription const& desc, VertexType const& vtype)
{
	fragmentShader = std::move(device->createFragmentShader(desc).value());
	vertexTypeFS = vtype;
}

void GraphicsPipeline::buildVertexShader(VertexShaderDescription const& desc, VertexType const& vtype)
{
	vertexShader = std::move(device->createVertexShader(desc).value());
	vertexTypeVS = vtype;
}

void GraphicsPipeline::registerVertexUniform(String const& name, BufferDescription bufferDesc, uint slot)
{
	auto bufferVal = device->createBuffer(bufferDesc);
	vertexUniformBuffers.push(UniformBindPoint{ name, std::move(bufferVal.value()), slot });
}

void GraphicsPipeline::setVertexUniform(String const& name, void* data, uint32_t size)
{
	auto bufferIt = std::ranges::find_if(vertexUniformBuffers, [name](auto const& e) {
			return e.name == name;
	});
	void* bufferData = device->mapBuffer(bufferIt->buffer);
		memcpy(bufferData, data, size);
	device->unmapBuffer(bufferIt->buffer);
}

void aes::GraphicsPipeline::registerFragmentUniform(String const& name, BufferDescription bufferDesc, uint slot)
{
	auto bufferVal = device->createBuffer(bufferDesc);
	fragmentUniformBuffers.push(UniformBindPoint{ name, std::move(bufferVal.value()), slot });
}

void aes::GraphicsPipeline::setFragmentUniform(String const& name, void* data, uint32_t size)
{
	auto bufferIt = std::ranges::find_if(fragmentUniformBuffers, [name](auto const& e) {
		return e.name == name;
		});
	void* bufferData = device->mapBuffer(bufferIt->buffer);
	memcpy(bufferData, data, size);
	device->unmapBuffer(bufferIt->buffer);
}
