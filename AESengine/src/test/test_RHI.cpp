#include "tests.hpp"

#include <glm/glm.hpp>
#include "core/allocator.hpp"
#include "core/color.hpp"
#include "renderer/RHI/RHIDevice.hpp"
#include "renderer/RHI/RHIRenderTarget.hpp"
#include "renderer/RHI/RHIShader.hpp"

using namespace aes;

class TestRHIApp
{
	RHIDevice device;
	RHIRenderTarget renderTarget;
	RHIFragmentShader fragmentShader;
	RHIVertexShader vertexShader;
	RHIBuffer vertexBuffer, indexBuffer;

public:

	TestRHIApp()
	{
		AES_LOG("[TEST] RHI");

		initializeGraphicsAPI();
		AES_LOG("graphics api initialized");

		// create device
		device.init();
		AES_LOG("device created successfully");

		// create RT
		RenderTargetDescription rtDesc = {};
		rtDesc.width = 960;
		rtDesc.height = 544;
		rtDesc.format = RHIFormat::R8G8B8A8_Uint;
		rtDesc.multisampleMode = MultisampleMode::None;
		renderTarget.init(rtDesc);

		aes::FragmentShaderDescription fragmentShaderDescription;
		static auto const clearShaderData_fs = aes::readFileBin("app0:assets/shaders/vita/clear_fs.gxp");
		fragmentShaderDescription.source = clearShaderData_fs.data();

		if (!fragmentShader.init(fragmentShaderDescription))
			AES_FATAL_ERROR("fragment shader creation failed");

		AES_LOG("fragment shader created");

		aes::VertexShaderDescription vertexShaderDescription;
		static auto const clearShaderData_vs = aes::readFileBin("app0:assets/shaders/vita/clear_vs.gxp");
		vertexShaderDescription.source = clearShaderData_vs.data();
		vertexShaderDescription.verticesStride = sizeof(glm::vec2);

		aes::VertexInputLayout vertexInputLayout[1];
		vertexInputLayout[0].parameterName = "aPosition";
		vertexInputLayout[0].semantic = aes::SemanticType::Position;
		vertexInputLayout[0].offset = 0;
		vertexInputLayout[0].format = aes::RHIFormat::R32G32_Float;

		vertexShaderDescription.verticesLayout = vertexInputLayout;

		if (!vertexShader.init(vertexShaderDescription))
			AES_FATAL_ERROR("vertex shader creation failed");

		AES_LOG("vertex shader created");

		glm::vec2 tri[] = {
			{-1.0f, -1.0f},
			{3.0f, -1.0f},
			{-1.0f, 3.0f},
		};

		aes::BufferDescription vertexBufferDesc = {};
		vertexBufferDesc.sizeInBytes = sizeof(glm::vec2) * 3;
		vertexBufferDesc.bindFlags = aes::BindFlagBits::VertexBuffer;
		vertexBufferDesc.cpuAccessFlags = CPUAccessFlagBits::None;
		vertexBufferDesc.usage = MemoryUsage::Immutable;
		vertexBufferDesc.initialData = tri;

		vertexBuffer.init(vertexBufferDesc);

		uint16_t indices[] = {1, 2, 3};

		aes::BufferDescription indexBufferDesc = {};
		indexBufferDesc.sizeInBytes = sizeof(uint16_t) * 3;
		indexBufferDesc.bindFlags = aes::BindFlagBits::IndexBuffer;
		indexBufferDesc.cpuAccessFlags = CPUAccessFlagBits::None;
		indexBufferDesc.usage = MemoryUsage::Immutable;
		indexBufferDesc.initialData = indices;

		indexBuffer.init(vertexBufferDesc);

		device.beginRenderPass(renderTarget);
			device.setVertexBuffer(vertexBuffer, sizeof(glm::vec2));
			device.setIndexBuffer(indexBuffer, IndexTypeFormat::Uint16);
			device.setFragmentShader(fragmentShader);
			device.setVertexShader(vertexShader);
			device.drawIndexed(3);
		device.endRenderPass();
	}

	void draw()
	{
		// begin render pass
			// bind shaders
			// bind buffers
			// draw
		// end render pass
	}

};

void aes::test_RHI()
{
	AES_START_PROFILE_SESSION("test RHI startup");
	TestRHIApp app;
	auto startupSession = AES_STOP_PROFILE_SESSION();

	AES_START_PROFILE_SESSION("test RHI running");
	auto runningSession = AES_STOP_PROFILE_SESSION();
}
