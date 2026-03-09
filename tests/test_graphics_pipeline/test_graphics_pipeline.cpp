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
#include "renderer/graphicsPipeline.hpp"
#include <iostream>
#include <cmath>
#include "renderer/simpleMeshes.hpp"

using namespace aes;

class TestPipelineApp : public Engine
{
public:

	RHIDevice device;
	RHISwapchain swapchain;

	RHIBuffer geoVertexBuffer, geoIndexBuffer;
	CameraBuffer cameraBuffer;

	GraphicsPipeline graphicsPipeline;
	vec3 camPos{0, 0, -3};
	vec3 front{0, 0, 1};
	vec3 up{ 0, 1, 0 };
	float camSpeed = 3;

	TestPipelineApp(InitInfo const& info) : Engine(info)
	{
		AES_LOG("[TEST] DRAW3D");
	}

	void start() override
	{
		initializeGraphicsAPI();
		AES_LOG("graphics api initialized");

		// create device
		device.init();
		device.setCullMode(CullMode::CounterClockwise);
		device.setDrawPrimitiveMode(DrawPrimitiveType::TrianglesFill);

		static TestPipelineApp* gApp = this;

		mainWindow->setResizeCallback([](uint w, uint h) {
			SwapchainDescription swDesc = {};
			swDesc.count = 2;
			swDesc.width = w;
			swDesc.height = h;
			swDesc.multisampleMode = MultisampleMode::None;
			swDesc.window = gApp->mainWindow->getHandle();
			swDesc.format = RHIFormat::R8G8B8A8_Uint;
			swDesc.depthFormat = RHIFormat::D24_S8_Uint;
			gApp->swapchain = gApp->device.createSwapchain(swDesc).value();
			gApp->cameraBuffer.proj = mat4::makePerspectiveProjectionMatD3D(60 * degToRad, float(w) / float(h), 0.01f, 100.0f);
			gApp->graphicsPipeline.setVertexUniform("camera", gApp->cameraBuffer);
		});

		{
			SwapchainDescription swDesc = {};
			swDesc.count = 2;
			mainWindow->getScreenSize(swDesc.width, swDesc.height);
			swDesc.multisampleMode = MultisampleMode::None;
			swDesc.window = mainWindow->getHandle();
			swDesc.format = RHIFormat::R8G8B8A8_Uint;
			swDesc.depthFormat = RHIFormat::D24_S8_Uint;
			swapchain = device.createSwapchain(swDesc).value();
		}
		{
			graphicsPipeline.init(&device);

			aes::VertexShaderDescription vertexShaderDescription;
			String vertexShaderPath = getEngineShaderPath();
			vertexShaderPath.append("/HLSL/draw3d.vs");
			vertexShaderDescription.source = readFile(vertexShaderPath);
			vertexShaderDescription.verticesStride = sizeof(aes::Vertex);

			vertexShaderDescription.verticesLayout.resize(2);

			vertexShaderDescription.verticesLayout[0].vertexBufferIndex = 0;
			vertexShaderDescription.verticesLayout[0].parameterName = "aPosition";
			vertexShaderDescription.verticesLayout[0].semantic = aes::SemanticType::Position;
			vertexShaderDescription.verticesLayout[0].offset = 0;
			vertexShaderDescription.verticesLayout[0].format = aes::RHIFormat::R32G32B32A32_Float;
			vertexShaderDescription.verticesLayout[0].classification = VertexInputClassification::PerVertex;

			vertexShaderDescription.verticesLayout[1].vertexBufferIndex = 0;
			vertexShaderDescription.verticesLayout[1].parameterName = "aColor";
			vertexShaderDescription.verticesLayout[1].semantic = aes::SemanticType::Color;
			vertexShaderDescription.verticesLayout[1].offset = sizeof(aes::vec4);
			vertexShaderDescription.verticesLayout[1].format = aes::RHIFormat::R32G32B32A32_Float;
			vertexShaderDescription.verticesLayout[1].classification = VertexInputClassification::PerVertex;

			graphicsPipeline.buildVertexShader(vertexShaderDescription);

			aes::FragmentShaderDescription fragmentShaderDescription;
			String fragmentShaderPath = getEngineShaderPath();
			fragmentShaderPath.append("/HLSL/draw3d.fs");
			fragmentShaderDescription.source = readFile(fragmentShaderPath);
			fragmentShaderDescription.multisampleMode = MultisampleMode::None;
			graphicsPipeline.buildFragmentShader(fragmentShaderDescription);

			AES_LOG("graphics pipeline created");
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
			geoIndexBufferDesc.sizeInBytes = sizeof(sm::cubeIndices);
			geoIndexBufferDesc.initialData = (void*)sm::cubeIndices;

			geoIndexBuffer = device.createBuffer(geoIndexBufferDesc).value();
		}
		{
			cameraBuffer.proj = mat4::makePerspectiveProjectionMatD3D(60 * degToRad, 960.0/544.0, 0.01f, 100.0f);

			cameraBuffer.view = mat4::makeLookAtMatrixLHD3D(camPos, vec3(0, 1, 0), vec3(0, 0, 1));

			aes::BufferDescription uniformBufferDesc;
			uniformBufferDesc.bindFlags = aes::BindFlagBits::UniformBuffer;
			uniformBufferDesc.usage = aes::MemoryUsage::Dynamic;
			uniformBufferDesc.cpuAccessFlags = aes::CPUAccessFlagBits::Write;
			uniformBufferDesc.sizeInBytes = sizeof(CameraBuffer);
			uniformBufferDesc.initialData = &cameraBuffer;

			graphicsPipeline.registerVertexUniform("camera", uniformBufferDesc, 0);
		}

		{
			mat4 model = mat4::identity();

			aes::BufferDescription uniformBufferDesc;
			uniformBufferDesc.bindFlags = aes::BindFlagBits::UniformBuffer;
			uniformBufferDesc.usage = aes::MemoryUsage::Dynamic;
			uniformBufferDesc.cpuAccessFlags = aes::CPUAccessFlagBits::Write;
			uniformBufferDesc.sizeInBytes = sizeof(mat4);
			uniformBufferDesc.initialData = &model;

			graphicsPipeline.registerVertexUniform("model", uniformBufferDesc, 1);
		}

		graphicsPipeline.bind();
		device.setVertexBuffer(geoVertexBuffer, 0, sizeof(Vertex));
		device.setIndexBuffer(geoIndexBuffer, IndexTypeFormat::Uint32);
	}

	int frontBufferIndex = 0, backBufferIndex = 0;
	float yaw = M_PI/2;
	float pitch = 0.0f;

	void update(float deltaTime) override
	{
		if (isKeyDown(Key::W))
		{
			camPos += front * camSpeed * deltaTime;
		}
		if (isKeyDown(Key::S))
		{
			camPos -= front * camSpeed * deltaTime;
		}
		if (isKeyDown(Key::A))
		{
			camPos -= vec3::cross(up, front) * camSpeed * deltaTime;
		}
		if (isKeyDown(Key::D))
		{
			camPos += vec3::cross(up, front) * camSpeed * deltaTime;
		}
		if (isKeyDown(Key::E))
		{
			camPos.y += camSpeed * deltaTime;
		}
		if (isKeyDown(Key::Q))
		{
			camPos.y -= camSpeed * deltaTime;
		}
		if (isKeyDown(Key::Right))
		{
			yaw -= camSpeed * deltaTime;
		}
		if (isKeyDown(Key::Left))
		{
			yaw += camSpeed * deltaTime;
		}
		if (isKeyDown(Key::Up))
		{
			pitch += camSpeed * deltaTime;
		}
		if (isKeyDown(Key::Down))
		{
			pitch -= camSpeed * deltaTime;
		}

		front.x = cos(yaw) * cos(pitch);
		front.y = sin(pitch);
		front.z = sin(yaw) * cos(pitch);
		front.normalize();

		//vec3 right = vec3::cross(front, vec3(0, 1, 0));  
		//right.normalize();
		//up = vec3::cross(front, right);
		//up.normalize();

		//vec4 front4 = mat4::rotateXYZMat(yaw, pitch,0) * vec4(front.x, front.y, front.z, 1);
		vec4 front4 = mat4::rotateYMat(yaw) * vec4(front.x, front.y, front.z, 1);

		vec4 up4 = mat4::rotateXYZMat(0, pitch, yaw) * vec4(up.x, up.y, up.z, 1);

		static mat4 model = mat4::identity();// = mat4::translationMat(vec3(0, 0, 0));
		mat4 tr = mat4::translationMat(vec3(0, 0, 2 * deltaTime));
		tr.transpose();
		//model = model * tr;
		mat4 rot = mat4::rotateYMat(1 * deltaTime);
		rot.transpose();
		model = model;// *rot;

		graphicsPipeline.setVertexUniform("model", model);

		cameraBuffer.view = mat4::makeLookAtMatrixLHD3D(camPos, up/*vec3(up4.x, up.y, up4.z)*/,front/*vec3(front4.x, front4.y, front4.z)*/);		
		graphicsPipeline.setVertexUniform("camera", cameraBuffer);
		AES_LOG("cam front x:{} y:{} z:{}", front.x, front.y, front.z);
	}

	void draw() override
	{
		device.clearSwapchain(swapchain);
		
		// draw
		device.beginRenderPass(swapchain);
		device.drawIndexed(std::size(sm::cubeIndices));
		device.endRenderPass();

		device.swapBuffers(swapchain);

		frontBufferIndex = backBufferIndex;
		backBufferIndex = (backBufferIndex + 1) % 2;
	}
};

int main()
{
	auto streamSink = aes::makeUnique<aes::StreamSink>(std::cout);
	aes::Logger::instance().addSink(streamSink.get());
	AES_START_PROFILE_SESSION("test draw3d startup");
	TestPipelineApp app({
		.appName = "aes draw3d test"
		});
	app.init();
	auto startupSession = AES_STOP_PROFILE_SESSION();
	printf("init time: %f", startupSession.elapsedSessionTime);

	//AES_START_PROFILE_SESSION("test draw3d running");
	app.run();
	//auto runningSession = AES_STOP_PROFILE_SESSION();

	return 0;
}
