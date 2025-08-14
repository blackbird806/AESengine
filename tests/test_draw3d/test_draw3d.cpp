#include "core/allocator.hpp"
#include "engine.hpp"
#include "core/color.hpp"
#include "core/vec3.hpp"
#include "core/vec4.hpp"
#include "core/matrix.hpp"
#include "core/uniquePtr.hpp"
#include "core/string.hpp"
#include "renderer/vertex.hpp"
#include "renderer/model.hpp"
#include "renderer/RHI/RHIDevice.hpp"
#include "renderer/RHI/RHIBuffer.hpp"
#include "renderer/RHI/RHIRenderTarget.hpp"
#include "renderer/RHI/RHIShader.hpp"

using namespace aes;

class TestDraw3dApp : public Engine
{
	RHIDevice device;
	RHISwapchain swapchain;

	RHIFragmentShader geoFragmentShader;
	RHIVertexShader geoVertexShader;
	RHIBuffer geoVertexBuffer, geoIndexBuffer;
	RHIBuffer uniformBuffer;

public:

	TestDraw3dApp(InitInfo const& info) : Engine(info)
	{
		AES_LOG("[TEST] DRAW3D");
	}

	void start() override
	{
		initializeGraphicsAPI();
		AES_LOG("graphics api initialized");

		// create device
		device.init();
		device.setCullMode(CullMode::None);
		device.setDrawPrimitiveMode(DrawPrimitiveType::TrianglesFill);

		{
			SwapchainDescription swDesc = {};
			swDesc.count = 2;
			swDesc.width = 960;
			swDesc.height = 544;
			swDesc.multisampleMode = MultisampleMode::X4;
			swDesc.window = mainWindow->getHandle();
			swDesc.format = RHIFormat::R8G8B8A8_Uint;
			swDesc.depthFormat = RHIFormat::D24_S8_Uint;
			swapchain = device.createSwapchain(swDesc).value();
		}
		{
			aes::FragmentShaderDescription fragmentShaderDescription;
			String shaderPath = getEngineShaderPath();
			shaderPath.append("/HLSL/draw3d.fs");
			fragmentShaderDescription.source = readFile(shaderPath.c_str());
			fragmentShaderDescription.multisampleMode = MultisampleMode::None;
			geoFragmentShader = device.createFragmentShader(fragmentShaderDescription).value();

			AES_LOG("fragment shader created");
		}
		{
			aes::VertexShaderDescription vertexShaderDescription;
			String shaderPath = getEngineShaderPath();
			shaderPath.append("/HLSL/draw3d.vs");

			vertexShaderDescription.source = readFile(shaderPath.c_str());
			vertexShaderDescription.verticesStride = sizeof(aes::Vertex);

			aes::VertexInputLayout vertexInputLayout[2];
			vertexInputLayout[0].parameterName = "aPosition";
			vertexInputLayout[0].semantic = aes::SemanticType::Position;
			vertexInputLayout[0].offset = 0;
			vertexInputLayout[0].format = aes::RHIFormat::R32G32B32A32_Float;

			vertexInputLayout[1].parameterName = "aColor";
			vertexInputLayout[1].semantic = aes::SemanticType::Color;
			vertexInputLayout[1].offset = sizeof(aes::vec4);
			vertexInputLayout[1].format = aes::RHIFormat::R32G32B32A32_Float;

			vertexShaderDescription.verticesLayout = vertexInputLayout;

			geoVertexShader = device.createVertexShader(vertexShaderDescription).value();

			AES_LOG("vertex shader created");
		}
		{
			Array<Vertex> vertexBufferData = getCubeVertices();
			aes::BufferDescription geoVertexBufferDesc;
			geoVertexBufferDesc.bindFlags = aes::BindFlagBits::VertexBuffer;
			geoVertexBufferDesc.cpuAccessFlags = aes::CPUAccessFlagBits::None;
			geoVertexBufferDesc.usage = aes::MemoryUsage::Immutable;
			geoVertexBufferDesc.sizeInBytes = vertexBufferData.size() * sizeof(Vertex);
			geoVertexBufferDesc.initialData = vertexBufferData.data();

			geoVertexBuffer = device.createBuffer(geoVertexBufferDesc).value();
		}
		{
			aes::BufferDescription geoIndexBufferDesc;
			geoIndexBufferDesc.bindFlags = aes::BindFlagBits::IndexBuffer;
			geoIndexBufferDesc.cpuAccessFlags = aes::CPUAccessFlagBits::None;
			geoIndexBufferDesc.usage = aes::MemoryUsage::Immutable;
			geoIndexBufferDesc.sizeInBytes = sizeof(cubeIndices);
			geoIndexBufferDesc.initialData = (void*)cubeIndices;
			geoVertexBuffer = device.createBuffer(geoIndexBufferDesc).value();
		}

		{
			aes::BufferDescription uniformBufferDesc;
			uniformBufferDesc.bindFlags = aes::BindFlagBits::UniformBuffer;
			uniformBufferDesc.usage = aes::MemoryUsage::Dynamic;
			uniformBufferDesc.cpuAccessFlags = aes::CPUAccessFlagBits::Write;
			uniformBufferDesc.sizeInBytes = sizeof(CameraBuffer);

			uniformBuffer = device.createBuffer(uniformBufferDesc).value();
		}
	}

	void update(float deltaTime) override
	{

	}

	void draw() override
	{
	}
};

int main()
{
	AES_START_PROFILE_SESSION("test draw3d startup");
	TestDraw3dApp app({
		.appName = "aes draw3d test"
		});
	app.init();
	auto startupSession = AES_STOP_PROFILE_SESSION();

	AES_START_PROFILE_SESSION("test draw3d running");
	app.run();
	auto runningSession = AES_STOP_PROFILE_SESSION();

	return 0;
}
