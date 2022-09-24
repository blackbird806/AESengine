#include "tests.hpp"

#include "core/allocator.hpp"
#include "engine.hpp"
#include "core/color.hpp"
#include "renderer/vertex.hpp"
#include "renderer/RHI/RHIDevice.hpp"
#include "renderer/RHI/RHIRenderTarget.hpp"

using namespace aes;

class TestRHIApp
{
	aes::RHIFragmentShader fragmentShader;
	aes::RHIVertexShader vertexShader;

public:

	TestRHIApp()
	{
		AES_LOG("[TEST] RHI");

		// create device

		// create RT

		aes::FragmentShaderDescription fragmentShaderDescription;
		fragmentShaderDescription.source = readFile("assets/shaders/HLSL/draw3d.fs");

		if (!fragmentShader.init(fragmentShaderDescription))
			AES_FATAL_ERROR("fragment shader creation failed");

		AES_LOG("fragment shader created");

		aes::VertexShaderDescription vertexShaderDescription;

		vertexShaderDescription.source = readFile("assets/shaders/HLSL/draw3d.vs");
		vertexShaderDescription.verticesStride = sizeof(aes::Vertex);

		aes::VertexInputLayout vertexInputLayout[2];
		vertexInputLayout[0].parameterName = "aPosition";
		vertexInputLayout[0].semantic = aes::SemanticType::Position;
		vertexInputLayout[0].offset = 0;
		vertexInputLayout[0].format = aes::RHIFormat::R32G32B32_Float;

		vertexInputLayout[1].parameterName = "aColor";
		vertexInputLayout[1].semantic = aes::SemanticType::Color;
		vertexInputLayout[1].offset = sizeof(glm::vec3);
		vertexInputLayout[1].format = aes::RHIFormat::R32G32B32A32_Float;

		vertexShaderDescription.verticesLayout = vertexInputLayout;

		if (!vertexShader.init(vertexShaderDescription))
			AES_FATAL_ERROR("vertex shader creation failed");

		AES_LOG("vertex shader created");
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
