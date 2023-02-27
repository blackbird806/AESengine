#include "tests.hpp"

#include <glm/glm.hpp>
#include "core/allocator.hpp"
#include "core/color.hpp"
#include "renderer/RHI/RHIDevice.hpp"
#include "renderer/RHI/RHIRenderTarget.hpp"
#include "renderer/RHI/RHIShader.hpp"

using namespace aes;

struct vert
{
	glm::vec2 pos;
	glm::vec3 col;
};

class TestRHIApp
{
	RHIDevice device;
	RHIRenderTarget renderTargets[2];
	RHIFragmentShader clearFragmentShader;
	RHIVertexShader clearVertexShader;

	RHIFragmentShader geoFragmentShader;
	RHIVertexShader geoVertexShader;
	RHIBuffer clearVertexBuffer, clearIndexBuffer;
	RHIBuffer geoVertexBuffer, geoIndexBuffer;

public:

	TestRHIApp()
	{
		AES_LOG("[TEST] RHI");

		initializeGraphicsAPI();
		AES_LOG("graphics api initialized");

		// create device
		device.init();
		AES_LOG("device created successfully");

		// create RTs
		for (size_t i = 0; i < std::size(renderTargets); i++)
		{
			RenderTargetDescription rtDesc = {};
			rtDesc.width = 960;
			rtDesc.height = 544;
			rtDesc.format = RHIFormat::R8G8B8A8_Uint;
			rtDesc.multisampleMode = MultisampleMode::None;
			renderTargets[i].init(rtDesc);
		}
		// clear init
		{
			aes::VertexShaderDescription vertexShaderDescription;
			auto const clearShaderData_vs = aes::readFileBin("app0:assets/shaders/vita/clear_vs.gxp");
			vertexShaderDescription.source = clearShaderData_vs.data();
			vertexShaderDescription.verticesStride = sizeof(glm::vec2);
			aes::VertexInputLayout vertexInputLayout[1];
			vertexInputLayout[0].parameterName = "aPosition";
			vertexInputLayout[0].semantic = aes::SemanticType::Position;
			vertexInputLayout[0].offset = 0;
			vertexInputLayout[0].format = aes::RHIFormat::R32G32_Float;

			vertexShaderDescription.verticesLayout = vertexInputLayout;

			if (!clearVertexShader.init(vertexShaderDescription))
				AES_FATAL_ERROR("vertex shader creation failed");

			AES_LOG("clear vertex shader created");
		}
		{
			aes::FragmentShaderDescription fragmentShaderDescription{};
			auto const clearShaderData_fs = aes::readFileBin("app0:assets/shaders/vita/clear_fs.gxp");
			fragmentShaderDescription.source = clearShaderData_fs.data();
			fragmentShaderDescription.multisampleMode = MultisampleMode::None;
			fragmentShaderDescription.gxpVertexProgram = clearVertexShader.getGxpShader();

			if (!clearFragmentShader.init(fragmentShaderDescription))
				AES_FATAL_ERROR("fragment shader creation failed");

			AES_LOG("clear fragment shader created");
		}
		{
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

			clearVertexBuffer.init(vertexBufferDesc);
			AES_LOG("clear vertex buffer created");
		}
		{
			uint16_t indices[] = {0, 1, 2};

			aes::BufferDescription indexBufferDesc = {};
			indexBufferDesc.sizeInBytes = sizeof(uint16_t) * 3;
			indexBufferDesc.bindFlags = aes::BindFlagBits::IndexBuffer;
			indexBufferDesc.cpuAccessFlags = CPUAccessFlagBits::None;
			indexBufferDesc.usage = MemoryUsage::Immutable;
			indexBufferDesc.initialData = indices;

			clearIndexBuffer.init(indexBufferDesc);
			AES_LOG("clear index buffer created");
		}
		// geometry init
		{
			aes::VertexShaderDescription vertexShaderDescription;
			auto const geoShaderData_vs = aes::readFileBin("app0:assets/shaders/vita/basic2d_vs.gxp");
			vertexShaderDescription.source = geoShaderData_vs.data();
			vertexShaderDescription.verticesStride = sizeof(glm::vec2);
			aes::VertexInputLayout vertexInputLayout[2];
			vertexInputLayout[0].parameterName = "aPosition";
			vertexInputLayout[0].semantic = aes::SemanticType::Position;
			vertexInputLayout[0].offset = 0;
			vertexInputLayout[0].format = aes::RHIFormat::R32G32_Float;

			vertexInputLayout[1].parameterName = "aColor";
			vertexInputLayout[1].semantic = aes::SemanticType::Color;
			vertexInputLayout[1].offset = sizeof(glm::vec2);
			vertexInputLayout[1].format = aes::RHIFormat::R32G32B32_Float;

			vertexShaderDescription.verticesLayout = vertexInputLayout;

			if (!geoVertexShader.init(vertexShaderDescription))
				AES_FATAL_ERROR("vertex shader creation failed");

			AES_LOG("geometry vertex shader created");
		}
		{
			aes::FragmentShaderDescription fragmentShaderDescription;
			auto const geoShaderData_fs = aes::readFileBin("app0:assets/shaders/vita/basic2d_fs.gxp");
			fragmentShaderDescription.source = geoShaderData_fs.data();
			fragmentShaderDescription.multisampleMode = MultisampleMode::None;
			fragmentShaderDescription.gxpVertexProgram = geoVertexShader.getGxpShader();

			if (!geoFragmentShader.init(fragmentShaderDescription))
				AES_FATAL_ERROR("fragment shader creation failed");

			AES_LOG("geometry fragment shader created");
		}
		{

			vert tri[] = {
				{{-0.25f, -0.25f}, {1.0f, 0.0f, 0.0f}},
				{{0.5f, -0.25f}, {0.0f, 1.0f, 0.0f}},
				{{-0.25f, 0.5f}, {0.0f, 0.0f, 1.0f}},
			};

			aes::BufferDescription vertexBufferDesc = {};
			vertexBufferDesc.sizeInBytes = sizeof(vert) * 3;
			vertexBufferDesc.bindFlags = aes::BindFlagBits::VertexBuffer;
			vertexBufferDesc.cpuAccessFlags = CPUAccessFlagBits::None;
			vertexBufferDesc.usage = MemoryUsage::Immutable;
			vertexBufferDesc.initialData = tri;

			geoVertexBuffer.init(vertexBufferDesc);
		}
		{
			uint16_t indices[] = {0, 1, 2};

			aes::BufferDescription indexBufferDesc = {};
			indexBufferDesc.sizeInBytes = sizeof(uint16_t) * 3;
			indexBufferDesc.bindFlags = aes::BindFlagBits::IndexBuffer;
			indexBufferDesc.cpuAccessFlags = CPUAccessFlagBits::None;
			indexBufferDesc.usage = MemoryUsage::Immutable;
			indexBufferDesc.initialData = indices;

			geoIndexBuffer.init(indexBufferDesc);
		}
	}

	void draw()
	{
		// clear
		device.setDrawPrimitiveMode(DrawPrimitiveType::TrianglesFill);
		device.setCullMode(CullMode::None);
		device.setVertexBuffer(clearVertexBuffer, sizeof(glm::vec2));
		device.setIndexBuffer(clearIndexBuffer, IndexTypeFormat::Uint16);
		device.setVertexShader(clearVertexShader);
		device.setFragmentShader(clearFragmentShader);

		device.beginRenderPass(renderTargets[0]);
			device.drawIndexed(3);
		device.endRenderPass();

		// draw
		device.setVertexBuffer(geoVertexBuffer, sizeof(vert));
		device.setIndexBuffer(geoIndexBuffer, IndexTypeFormat::Uint16);
		device.setVertexShader(geoVertexShader);
		device.setFragmentShader(geoFragmentShader);

		device.beginRenderPass(renderTargets[0]);
			device.drawIndexed(3);
		device.endRenderPass();

		device.swapBuffers(renderTargets[0], renderTargets[0]);
	}

};

void aes::test_RHI()
{
	AES_START_PROFILE_SESSION("test RHI startup");
	TestRHIApp app;
	auto startupSession = AES_STOP_PROFILE_SESSION();

	//while (1)
	{
		app.draw();
	}

	AES_START_PROFILE_SESSION("test RHI running");
	auto runningSession = AES_STOP_PROFILE_SESSION();
}
