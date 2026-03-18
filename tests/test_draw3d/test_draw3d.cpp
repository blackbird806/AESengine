#include "core/allocator.hpp"
#include "engine.hpp"
#include "renderer/draw3d.hpp"

#include "core/matrix.hpp"
#include "renderer/vertex.hpp"
#include <iostream>

using namespace wob;

class TestDraw3dApp : public Engine
{
public:

	RHIDevice device;
	RHISwapchain swapchain;
	CameraBuffer cameraBuffer;

	Draw3D draw3d;
	
	vec3 camPos{ 0, 0, -3 };
	vec3 front{ 0, 0, 1 };
	vec3 up{ 0, 1, 0 };
	float camSpeed = 3;

	TestDraw3dApp(InitInfo const& info) : Engine(info)
	{
		WOB_LOG("[TEST] DRAW3D");
	}

	void start() override
	{
		initializeGraphicsAPI();
		WOB_LOG("graphics api initialized");

		// create device
		device.init();
		device.setCullMode(CullMode::None);
		draw3d.init(device);

		static TestDraw3dApp* gApp = this;

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
			gApp->draw3d.pipeline.setVertexUniform("camera", gApp->cameraBuffer);
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
			cameraBuffer.proj = mat4::makePerspectiveProjectionMatD3D(60 * degToRad, 960.0 / 544.0, 0.01f, 100.0f);

			cameraBuffer.view = mat4::makeLookAtMatrixLHD3D(camPos, vec3(0, 1, 0), vec3(0, 0, 1));

			wob::BufferDescription uniformBufferDesc;
			uniformBufferDesc.bindFlags = wob::BindFlagBits::UniformBuffer;
			uniformBufferDesc.usage = wob::MemoryUsage::Dynamic;
			uniformBufferDesc.cpuAccessFlags = wob::CPUAccessFlagBits::Write;
			uniformBufferDesc.sizeInBytes = sizeof(CameraBuffer);
			uniformBufferDesc.initialData = &cameraBuffer;

			draw3d.pipeline.registerVertexUniform("camera", uniformBufferDesc, 0);
		}
	}

	int frontBufferIndex = 0, backBufferIndex = 0;
	float yaw = M_PI / 2;
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

		cameraBuffer.view = mat4::makeLookAtMatrixLHD3D(camPos, up, front);
		draw3d.pipeline.setVertexUniform("camera", cameraBuffer);
	}

	void draw() override
	{
		device.clearSwapchain(swapchain);

		// draw
		device.beginRenderPass(swapchain);
		draw3d.setColor(Color::Blue);
		draw3d.drawLine(Line3D{ vec3(0.0f), vec3(0.0f, 0.0f, 10.0f) });
		draw3d.setColor(Color::Red);
		draw3d.drawLine(Line3D{ vec3(0.0f), vec3(10.0f, 0.0f, 0.0f) });
		draw3d.setColor(Color::Green);
		draw3d.drawLine(Line3D{ vec3(0.0f), vec3(0.0f, 10.0f, 0.0f) });
		draw3d.executeDrawCommands();
		device.endRenderPass();

		device.swapBuffers(swapchain);

		frontBufferIndex = backBufferIndex;
		backBufferIndex = (backBufferIndex + 1) % 2;
	}
};

int main()
{
	auto streamSink = wob::makeUnique<wob::StreamSink>(std::cerr);
	wob::Logger::instance().addSink(streamSink.get());
	WOB_START_PROFILE_SESSION("test draw3d startup");
	TestDraw3dApp app({
		.appName = "aes draw3d test"
		});
	app.init();
	auto startupSession = WOB_STOP_PROFILE_SESSION();
	printf("init time: %f", startupSession.elapsedSessionTime);

	//WOB_START_PROFILE_SESSION("test draw3d running");
	app.run();
	//auto runningSession = WOB_STOP_PROFILE_SESSION();

	return 0;
}
