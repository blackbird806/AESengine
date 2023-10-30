#include "tests.hpp"

#include "core/allocator.hpp"
#include "core/color.hpp"
#include "core/vec2.hpp"
#include "core/vec3.hpp"
#include "core/uniquePtr.hpp"
#include "core/string.hpp"
#include "renderer/RHI/RHIDevice.hpp"
#include "renderer/RHI/RHIBuffer.hpp"
#include "renderer/RHI/RHIRenderTarget.hpp"
#include "renderer/RHI/RHIShader.hpp"
#ifdef _WIN32
#include "core/platformWindows/win_window.hpp"
#else
#include "core/window.hpp"
#endif
#include "core/hashmap.hpp"

using namespace aes;

struct vert
{
	aes::vec2 pos;
	aes::vec3 col;
};

class TestRHIApp
{
	UniquePtr<Window> window;
	RHIDevice device;
	RHISwapchain swapchain;
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

		HashMap<String, int> map(16);
		map.add(String("hello"), 15);
		map["hello"] = 16;

#ifdef _WIN32
		window = makeUnique<Win_Window>("aes engine");
#else
		window = makeUnique<EmptyWindow>();
#endif
		initializeGraphicsAPI();
		AES_LOG("graphics api initialized");

		// create device
		device.init();
		AES_LOG("device created successfully");

		{
			SwapchainDescription swDesc = {};
			swDesc.count = 2;
			swDesc.width = 960;
			swDesc.height = 544;
			swDesc.multisampleMode = MultisampleMode::None;
			swDesc.window = window->getHandle();
			swDesc.format = RHIFormat::R8G8B8A8_Uint;
			swDesc.depthFormat = RHIFormat::D24_S8_Uint;
			swapchain = device.createSwapchain(swDesc).value();
		}
		// clear init
#ifdef __vita__
		{
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
			clearVertexShader = device.createVertexShader(vertexShaderDescription).value();

			AES_LOG("clear vertex shader created");
		}
		{
			aes::FragmentShaderDescription fragmentShaderDescription{};
			static auto const clearShaderData_fs = aes::readFileBin("app0:assets/shaders/vita/clear_fs.gxp");
			fragmentShaderDescription.source = clearShaderData_fs.data();
			fragmentShaderDescription.multisampleMode = MultisampleMode::None;
			//fragmentShaderDescription.gxpVertexProgram = clearVertexShader.getGxpShader();

			clearFragmentShader = device.createFragmentShader(fragmentShaderDescription).value();

			AES_LOG("clear fragment shader created");
		}
#endif

		{
			aes::vec2 tri[] = {
				{-1.0f, -1.0f},
				{3.0f, -1.0f},
				{-1.0f, 3.0f},
			};

			aes::BufferDescription vertexBufferDesc = {};
			vertexBufferDesc.sizeInBytes = sizeof(aes::vec2) * 3;
			vertexBufferDesc.bindFlags = aes::BindFlagBits::VertexBuffer;
			vertexBufferDesc.cpuAccessFlags = CPUAccessFlagBits::None;
			vertexBufferDesc.usage = MemoryUsage::Immutable;
			vertexBufferDesc.initialData = tri;

			clearVertexBuffer = device.createBuffer(vertexBufferDesc).value();
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

			clearIndexBuffer = device.createBuffer(indexBufferDesc).value();
			AES_LOG("clear index buffer created");
		}
		// geometry init
		{
			aes::VertexShaderDescription vertexShaderDescription;
#ifdef __vita__
			static auto const geoShaderData_vs = aes::readFileBin("app0:assets/shaders/vita/basic2d_vs.gxp");
			vertexShaderDescription.source = geoShaderData_vs.data();
#else
			vertexShaderDescription.source = aes::readFile("assets/shaders/HLSL/draw2d.vs");
#endif
			vertexShaderDescription.verticesStride = sizeof(aes::vec2);
			aes::VertexInputLayout vertexInputLayout[2];
			vertexInputLayout[0].parameterName = "aPosition";
			vertexInputLayout[0].semantic = aes::SemanticType::Position;
			vertexInputLayout[0].offset = 0;
			vertexInputLayout[0].format = aes::RHIFormat::R32G32_Float;

			vertexInputLayout[1].parameterName = "aColor";
			vertexInputLayout[1].semantic = aes::SemanticType::Color;
			vertexInputLayout[1].offset = sizeof(aes::vec2);
			vertexInputLayout[1].format = aes::RHIFormat::R32G32B32_Float;

			vertexShaderDescription.verticesLayout = vertexInputLayout;

			geoVertexShader = device.createVertexShader(vertexShaderDescription).value();

			AES_LOG("geometry vertex shader created");
		}
		{
			aes::FragmentShaderDescription fragmentShaderDescription;
#ifdef __vita__
			// shader binary source must be keep into memory
			static auto const geoShaderData_fs = aes::readFileBin("app0:assets/shaders/vita/basic2d_fs.gxp");
			fragmentShaderDescription.source = geoShaderData_fs.data();
			fragmentShaderDescription.gxpVertexProgram = geoVertexShader.getGxpShader();
#else
			fragmentShaderDescription.source = aes::readFile("assets/shaders/HLSL/draw2d.fs");
#endif
			fragmentShaderDescription.multisampleMode = MultisampleMode::None;

			geoFragmentShader = device.createFragmentShader(fragmentShaderDescription).value();

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

			geoVertexBuffer = device.createBuffer(vertexBufferDesc).value();
			AES_LOG("geoVertexBuffer created");
		}
		{
			uint16_t indices[] = {0, 1, 2};

			aes::BufferDescription indexBufferDesc = {};
			indexBufferDesc.sizeInBytes = sizeof(uint16_t) * 3;
			indexBufferDesc.bindFlags = aes::BindFlagBits::IndexBuffer;
			indexBufferDesc.cpuAccessFlags = CPUAccessFlagBits::None;
			indexBufferDesc.usage = MemoryUsage::Immutable;
			indexBufferDesc.initialData = indices;

			geoIndexBuffer = device.createBuffer(indexBufferDesc).value();
			AES_LOG("indexBufferDesc created");
		}
	}

	int frontBufferIndex = 0, backBufferIndex = 0;
	float xx = -0.25f;

	void draw()
	{
#ifdef __vita__
		// clear
		device.setDrawPrimitiveMode(DrawPrimitiveType::TrianglesFill);
		device.setCullMode(CullMode::None);
		device.setVertexBuffer(clearVertexBuffer, sizeof(glm::vec2));
		device.setIndexBuffer(clearIndexBuffer, IndexTypeFormat::Uint16);
		device.setVertexShader(clearVertexShader);
		device.setFragmentShader(clearFragmentShader);

		device.beginRenderPass(swapchain);
			device.drawIndexed(3);
		device.endRenderPass();
#endif
		vert tri[] = {
				{{xx, -0.25f}, {1.0f, 0.0f, 0.0f}},
				{{0.5f, -0.25f}, {0.0f, 1.0f, 0.0f}},
				{{-0.25f, 0.5f}, {0.0f, 0.0f, 1.0f}},
			};

		xx += 0.001f;
		if (auto map = device.mapBuffer(geoVertexBuffer))
		{
			memcpy(map.value(), &tri, sizeof(tri));
			device.unmapBuffer(geoVertexBuffer);
		}
		else
		{
			AES_FATAL_ERROR("Failed to map geoVertexBuffer !");
		}
		// draw
		device.setVertexBuffer(geoVertexBuffer, sizeof(vert));
		device.setIndexBuffer(geoIndexBuffer, IndexTypeFormat::Uint16);
		device.setVertexShader(geoVertexShader);
		device.setFragmentShader(geoFragmentShader);
		
		device.beginRenderPass(swapchain);
			device.drawIndexed(3);
		device.endRenderPass();

		device.swapBuffers(swapchain);

		frontBufferIndex = backBufferIndex;
		backBufferIndex = (backBufferIndex + 1) % 2;
	}

};

void aes::test_RHI()
{
	AES_START_PROFILE_SESSION("test RHI startup");
	TestRHIApp app;
	auto startupSession = AES_STOP_PROFILE_SESSION();
	
	AES_LOG("Draw start");
	while (1)
	{
		app.draw();
	}

	AES_START_PROFILE_SESSION("test RHI running");
	auto runningSession = AES_STOP_PROFILE_SESSION();
}
