//#include "tests.hpp"
//
//#include "core/allocator.hpp"
//#include "engine.hpp"
//#include "core/color.hpp"
//#include "renderer/material.hpp"
//#include "renderer/vertex.hpp"
//
//using namespace aes;
//
//class TestDraw3dApp : public Engine
//{
//	aes::RHIFragmentShader fragmentShader;
//	aes::RHIVertexShader vertexShader;
//	aes::RHIBuffer viewBuffer;
//	aes::RHIBuffer identityModelBuffer;
//	aes::Material defaultMtrl;
//
//public:
//
//	TestDraw3dApp(InitInfo const& info) : Engine(info)
//	{
//		AES_LOG("[TEST] DRAW3D");
//	}
//
//	void start() override
//	{
//		aes::FragmentShaderDescription fragmentShaderDescription;
//		fragmentShaderDescription.source = readFile("assets/shaders/HLSL/draw3d.fs");
//
//		if (!fragmentShader.init(fragmentShaderDescription))
//			AES_FATAL_ERROR("fragment shader creation failed");
//
//		AES_LOG("fragment shader created");
//
//		aes::VertexShaderDescription vertexShaderDescription;
//
//		vertexShaderDescription.source = readFile("assets/shaders/HLSL/draw3d.vs");
//		vertexShaderDescription.verticesStride = sizeof(aes::Vertex);
//
//		aes::VertexInputLayout vertexInputLayout[2];
//		vertexInputLayout[0].parameterName = "aPosition";
//		vertexInputLayout[0].semantic = aes::SemanticType::Position;
//		vertexInputLayout[0].offset = 0;
//		vertexInputLayout[0].format = aes::RHIFormat::R32G32B32_Float;
//
//		vertexInputLayout[1].parameterName = "aColor";
//		vertexInputLayout[1].semantic = aes::SemanticType::Color;
//		vertexInputLayout[1].offset = sizeof(glm::vec3);
//		vertexInputLayout[1].format = aes::RHIFormat::R32G32B32A32_Float;
//
//		vertexShaderDescription.verticesLayout = vertexInputLayout;
//
//		if (!vertexShader.init(vertexShaderDescription))
//			AES_FATAL_ERROR("vertex shader creation failed");
//
//		AES_LOG("vertex shader created");
//
//		if (!defaultMtrl.init(&vertexShader, &fragmentShader))
//			AES_FATAL_ERROR("material creation failed");
//
//		AES_LOG("material created");
//	}
//
//	void update(float deltaTime) override
//	{
//
//	}
//
//	void draw() override
//	{
//	}
//};
//
//void aes::test_draw3d()
//{
//	AES_START_PROFILE_SESSION("test draw3d startup");
//	TestDraw3dApp app({
//		.appName = "aes draw3d test"
//		});
//	app.init();
//	auto startupSession = AES_STOP_PROFILE_SESSION();
//
//	AES_START_PROFILE_SESSION("test draw3d running");
//	app.run();
//	auto runningSession = AES_STOP_PROFILE_SESSION();
//}
