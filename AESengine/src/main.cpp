//#include <glm/gtx/transform.hpp>
//#include <iostream>
//#include <fstream>
//#include "core/profiler.hpp"
//#include "core/debugMath.hpp"
//#include "engine.hpp"
//#include "renderer/model.hpp"
//#include "renderer/material.hpp"
//#include "core/os.hpp"
//#include "core/utility.hpp"
//
//#include "aini/aini.hpp"
//
//const char pxShader[] = R"(
//struct VS_OUTPUT
//{
//    float4 position : SV_POSITION;
//    float4 color : COLOR;
//};
//
//float4 main(VS_OUTPUT input) : SV_TARGET
//{
//    return input.color;
//}
//)";
//
//const char vShader[] = R"(
//struct VS_INPUT
//{
//    float4 position : POSITION;
//    float4 color : COLOR;
//};
//
//struct VS_OUTPUT
//{
//    float4 position : SV_POSITION;
//    float4 color : COLOR;
//};
//
//cbuffer CameraBuffer
//{
//	float4x4 viewMatrix;
//	float4x4 projectionMatrix;
//};
//
//cbuffer ModelBuffer
//{
//	float4x4 worldMatrix;
//};
//
//VS_OUTPUT main(VS_INPUT input)
//{
//    VS_OUTPUT output;
//
//    // Change the position vector to be 4 units for proper matrix calculations.
//    input.position.w = 1.0f;
//
//    // Calculate the position of the vertex against the world, view, and projection matrices.
//    output.position = mul(input.position, worldMatrix);
//    output.position = mul(output.position, viewMatrix);
//    output.position = mul(output.position, projectionMatrix);
//    //output.position = input.position;
//
//    // Store the input color for the pixel shader to use.
//    output.color = input.color;
//    
//    return output;
//})";
//
//class Game : public aes::Engine
//{
//
//public:
//
//	aes::RHIFragmentShader fragmentShader;
//	aes::RHIVertexShader vertexShader;
//	aes::RHIBuffer viewBuffer;
//	aes::Material defaultMtrl;
//	aes::Model model;
//	
//	Game(InitInfo const& info) : Engine(info)
//	{
//		AES_LOG("Game initialized");
//	}
//
//	void start() override
//	{
//		AES_PROFILE_FUNCTION();
//		AES_LOG("start");
//
//		aes::FragmentShaderDescription fragmentShaderDescription;
//		fragmentShaderDescription.source = pxShader;
//
//		if (!fragmentShader.init(fragmentShaderDescription))
//		{
//			AES_ASSERTF(false, "fragment shader creation failed");
//		}
//
//		AES_LOG("fragment shader created");
//
//		aes::VertexShaderDescription vertexShaderDescription;
//
//		vertexShaderDescription.source = vShader;
//		vertexShaderDescription.verticesStride = sizeof(aes::Vertex);
//
//		aes::VertexInputLayout vertexInputLayout[2];
//		vertexInputLayout[0].semantic = aes::SemanticType::Position;
//		vertexInputLayout[0].offset = 0;
//		vertexInputLayout[0].format = aes::RHIFormat::R32G32B32_Float;
//
//		vertexInputLayout[1].semantic = aes::SemanticType::Color;
//		vertexInputLayout[1].offset = sizeof(glm::vec3);
//		vertexInputLayout[1].format = aes::RHIFormat::R32G32B32A32_Float;
//
//		vertexShaderDescription.verticesLayout = vertexInputLayout;
//
//		if (!vertexShader.init(vertexShaderDescription))
//		{
//			AES_ASSERTF(false, "vertex shader creation failed");
//		}
//		AES_LOG("vertex shader created");
//
//		if (!defaultMtrl.init(&vertexShader, &fragmentShader))
//		{
//			AES_ASSERTF(false, "material creation failed");
//		}
//		AES_LOG("material created");
//
//		model = aes::createCube().value();
//		AES_LOG("cube created");
//
//		aes::BufferDescription viewDesc;
//		viewDesc.bindFlags = aes::BindFlagBits::UniformBuffer;
//		viewDesc.bufferUsage = aes::BufferUsage::Dynamic;
//		viewDesc.cpuAccessFlags = aes::CPUAccessFlagBits::Write;
//		viewDesc.sizeInBytes = sizeof(aes::CameraBuffer);
//		viewBuffer.init(viewDesc);
//		AES_LOG("view buffer created");
//
//		mainCamera.pos = { 0.0, 0.0, -5.0 };
//		getViewportMousePos(lastMousePosX, lastMousePosY);
//	}
//
//	float speed = 5.0f, sensitivity = 50.f;
//	float lastMousePosX, lastMousePosY;
//	glm::vec3 direction = {0.0, 0.0, 1.0};
//	float yaw = 91, pitch = 2;
//	
//	void update(float dt) override
//	{
//		AES_PROFILE_FUNCTION();
//		glm::vec4 movePos = { 0.0f, 0.f, 0.f, 0.0f };
//		if (getKeyState(aes::Key::W) == aes::InputState::Down)
//		{
//			movePos.z += speed * dt;
//		}
//
//		if (getKeyState(aes::Key::S) == aes::InputState::Down)
//		{
//			movePos.z -= speed * dt;
//		}
//
//		if (getKeyState(aes::Key::D) == aes::InputState::Down)
//		{
//			movePos.x += speed * dt;
//		}
//
//		if (getKeyState(aes::Key::A) == aes::InputState::Down)
//		{
//			movePos.x -= speed * dt;
//		}
//
//		if (getKeyState(aes::Key::E) == aes::InputState::Down)
//		{
//			movePos.y += speed * dt;
//		}
//
//		if (getKeyState(aes::Key::Q) == aes::InputState::Down)
//		{
//			movePos.y -= speed * dt;
//		}
//
//		mainCamera.pos += glm::vec3(movePos * mainCamera.viewMatrix);
//		float mx, my;
//		getViewportMousePos(mx, my);
//		
//		if (getKeyState(aes::Key::RClick) == aes::InputState::Down)
//		{
//			float xoffset = mx - lastMousePosX;
//			float yoffset = my - lastMousePosY; // reversed since y-coordinates range from bottom to top
//
//			lastMousePosX = mx;
//			lastMousePosY = my;
//
//			xoffset *= sensitivity;
//			yoffset *= sensitivity;
//
//			yaw += xoffset;
//			pitch += yoffset;
//
//			if (pitch > 89.0f)
//				pitch = 89.0f;
//			if (pitch < -89.0f)
//				pitch = -89.0f;
//
//			direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
//			direction.y = sin(glm::radians(pitch));
//			direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
//		}
//		else
//		{
//			getViewportMousePos(lastMousePosX, lastMousePosY);
//		}
//		mainCamera.lookAt(mainCamera.pos + glm::normalize(direction));
//
//		{
//			float const ex = 0.0055f;
//			float const csx = 0.025f;
//
//			uint windowWidth = 960, windowHeight = 544;
//			mainWindow->getScreenSize(windowWidth, windowHeight);
//			float const aspect = (float)windowWidth / (float)windowHeight;
//			mainCamera.projMatrix = glm::perspectiveLH_ZO(glm::radians(45.0f), aspect, 0.0001f, 1000.0f);
//		}
//		model.toWorld = glm::rotate(model.toWorld, 1.5f * dt, glm::vec3(0.0f, 1.0f, 1.0f));
//		aes::CameraBuffer const camBuf{ glm::transpose(mainCamera.viewMatrix), glm::transpose(mainCamera.projMatrix) };
//		//aes::CameraBuffer const camBuf { mainCamera.viewMatrix, mainCamera.projMatrix };
//		viewBuffer.setDataFromPOD(camBuf);
//	}
//
//	void draw() override
//	{
//		AES_PROFILE_FUNCTION();
//		
//		//draw2d.setColor(aes::Color::Blue);
//		//draw2d.drawPoint({0.f, 0.f});
//		////draw2d.drawPoint({1.0f, 1.0f});
//		//draw2d.setColor(aes::Color::Red);
//		//draw2d.drawPoint({ 0.5f, 0.5f });
//
//		//draw2d.executeDrawCommands();
//		aes::Material::BindInfo bindInfo;
//		bindInfo.vsBuffers.push_back(std::make_pair("ModelBuffer", &model.modelBuffer));
//		bindInfo.vsBuffers.push_back(std::make_pair("CameraBuffer", &viewBuffer));
//		defaultMtrl.bind(bindInfo);
//		model.draw();
//	}
//};
//
//int main()
//{
//	std::string appName = "aes engine";
//	
//	std::ofstream logFile("AES_log.txt");
//	aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>(std::cout));
//
//	aini::Reader engineConfig(aes::readFile("engine.ini"));
//	if (engineConfig.has_key("app_name"))
//		appName = engineConfig.get_string("app_name");
//	aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>(logFile));
//	AES_START_PROFILE_SESSION("startup");
//	
//	Game game({
//		.appName = appName.c_str()
//	});
//
//	game.init();
//	auto startupSession = AES_STOP_PROFILE_SESSION();
//	
//	AES_START_PROFILE_SESSION("running");
//	game.run();
//	auto runningSession = AES_STOP_PROFILE_SESSION();
//	
//	std::ofstream timmingFile("prof.txt");
//	for (auto const& [_, v] : startupSession.profileDatas)
//	{
//		timmingFile << fmt::format("{}\n\ttotalTime:{}ms\n\tcount:{}\n\taverage:{}ms\n\tparent:{}\n",
//			v.name, v.elapsedTime, v.count, v.elapsedTime / v.count, v.parentName != nullptr ? v.parentName : "null");
//	}
//
//	for (auto const& [_, v] : runningSession.profileDatas)
//	{
//		timmingFile << fmt::format("{}\n\ttotalTime:{}ms\n\tcount:{}\n\taverage:{}ms\n\tparent:{}\n", 
//			v.name, v.elapsedTime, v.count, v.elapsedTime / v.count, v.parentName != nullptr ? v.parentName : "null");
//	}
//	
//	return 0;
//}
#include <glm/gtx/transform.hpp>
#include <iostream>
#include <fstream>
#include "core/profiler.hpp"
#include "core/debugMath.hpp"
#include "engine.hpp"
#include "core/os.hpp"
#include "core/utility.hpp"
#include "core/color.hpp"
#include "renderer/draw2d.hpp"

#include "aini/aini.hpp"

class Game : public aes::Engine
{

public:

	aes::RHIBuffer vertexBuffer;
	aes::RHIFragmentShader fragmentShader;
	aes::RHIVertexShader vertexShader;
	aes::Draw2d draw2d;

	Game(InitInfo const& info) : Engine(info)
	{
		AES_LOG("Game initialized");
	}

	struct Vertex
	{
		glm::vec2 pos;
		glm::vec4 color;
	};


	void start() override
	{
		AES_PROFILE_FUNCTION();
		AES_LOG("start");
		using namespace aes;
		draw2d.init();

		// Vertex basicVertices[3];
		// basicVertices[0].pos = {-0.5f, -0.5f};
		// basicVertices[0].color = {1.0f, 0.0f, 0.0f, 1.0f};

		// basicVertices[1].pos = {0.5f, -0.5f};
		// basicVertices[1].color = {1.0f, 0.0f, 0.0f, 1.0f};

		// basicVertices[2].pos = {-0.5f, 0.5f};
		// basicVertices[2].color = {1.0f, 0.0f, 0.0f, 1.0f};

		// aes::BufferDescription vertexBufferDescription;
		// vertexBufferDescription.sizeInBytes = sizeof(basicVertices);
		// vertexBufferDescription.bufferUsage = aes::BufferUsage::Dynamic;
		// vertexBufferDescription.cpuAccessFlags = aes::CPUAccessFlagBits::Write;
		// vertexBufferDescription.bindFlags = aes::BindFlagBits::VertexBuffer;
		// vertexBuffer.init(vertexBufferDescription);

		// aes::VertexInputLayout vertexInputLayout[2];
		// vertexInputLayout[0].semantic = SemanticType::Position;
		// vertexInputLayout[0].offset = 0;
		// vertexInputLayout[0].format = RHIFormat::R32G32_Float;

		// vertexInputLayout[1].semantic = SemanticType::Color;
		// vertexInputLayout[1].offset = sizeof(glm::vec2);
		// vertexInputLayout[1].format = RHIFormat::R32G32B32A32_Float;

		// AES_LOG("so far");
		// aes::VertexShaderDescription vertexShaderDescription;
		// auto const source_vs = aes::readFileBin("app0:assets/shaders/vita/basic2d_vs.gxp");
		// vertexShaderDescription.source = source_vs.data();
		// AES_LOG("alt {}", vertexShaderDescription.source.index());
		// AES_LOG("holds {}", std::holds_alternative<uint8_t const*>(vertexShaderDescription.source));
		// vertexShaderDescription.verticesLayout = vertexInputLayout;
		// vertexShaderDescription.verticesStride = sizeof(Vertex);
		// AES_LOG("shader 1 start");

		// auto err = vertexShader.init(vertexShaderDescription);
		// AES_LOG("shader 1 initialized");
		// aes::FragmentShaderDescription fragmentShaderDescription;
		// auto const source_fs = aes::readFileBin("app0:assets/shaders/vita/basic2d_fs.gxp");
		// fragmentShaderDescription.source = source_fs.data();
		// err = fragmentShader.init(fragmentShaderDescription);
		// AES_LOG("shader 2 initialized");
		AES_DEBUG_BREAK();
	}


	void update(float dt) override
	{
		AES_PROFILE_FUNCTION();

	}

	void draw() override
	{
		AES_PROFILE_FUNCTION();
		draw2d.setColor(aes::Color::Blue);
		//draw2d.drawPoint({ 0.0f, 0.0f }, 100.0f);
		//draw2d.drawPoint({ 0.5f, 0.5f }, 100.0f);
		draw2d.drawLine({{0.0f, 0.0f}, {100.0f, 100.0f}});
		draw2d.executeDrawCommands();
	}
};

int main()
{
	std::string appName = "aes engine";
	//std::ofstream logFile("ux0:log/AES_log.txt");
	// aes::Logger::instance().addSink(std::make_unique<aes::PsvDebugScreenSink>());
	//aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>(logFile));
	aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>(std::cerr));

	Game game({
		.appName = appName.c_str()
		});

	game.init();
	game.run();

	return 0;
}
