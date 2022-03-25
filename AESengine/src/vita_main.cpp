#include <iostream>
#include <fstream>
#include <glm/gtx/transform.hpp>
#include "core/profiler.hpp"
#include "core/debugMath.hpp"
#include "engine.hpp"
#include "core/os.hpp"
#include "core/utility.hpp"
#include "core/color.hpp"
#include "renderer/RHI/RHIRenderContext.hpp"
#include "renderer/draw2d.hpp"
#include "renderer/RHI/RHITexture.hpp"
#include "renderer/textureUtility.hpp"
#include "renderer/model.hpp"
#include "renderer/camera.hpp"

class Game : public aes::Engine
{

public:

	aes::RHIBuffer vertexBuffer;
	aes::RHIBuffer indexBuffer;
	aes::RHIBuffer viewBuffer, modelBuffer;
	aes::RHITexture texture;
	aes::RHIFragmentShader fragmentShader;
	aes::RHIVertexShader vertexShader;
	aes::Camera camera;

	Game(InitInfo const& info) : Engine(info)
	{
		AES_LOG("Game initialized");
	}

	void start() override
	{
		using namespace aes;

		AES_PROFILE_FUNCTION();
		AES_LOG("start");

		auto vertices = getCubeVertices();

		{
			aes::BufferDescription vertexBufferDescription;
			vertexBufferDescription.sizeInBytes = sizeof(aes::Vertex) * vertices.size();
			vertexBufferDescription.usage = aes::MemoryUsage::Dynamic;
			vertexBufferDescription.cpuAccessFlags = aes::CPUAccessFlagBits::Write;
			vertexBufferDescription.bindFlags = aes::BindFlagBits::VertexBuffer;
			vertexBufferDescription.initialData = vertices.data();
			vertexBuffer.init(vertexBufferDescription);
		}

		uint16_t indices[36];
		for (int i = 0; auto& index : indices)
			index = cubeIndices[i++];
		{
			aes::BufferDescription indexBufferDescription;
			indexBufferDescription.sizeInBytes = sizeof(indices);
			indexBufferDescription.usage = aes::MemoryUsage::Dynamic;
			indexBufferDescription.cpuAccessFlags = aes::CPUAccessFlagBits::Write;
			indexBufferDescription.bindFlags = aes::BindFlagBits::IndexBuffer;
			indexBufferDescription.initialData = indices;
			indexBuffer.init(indexBufferDescription);
		}

		{
			aes::BufferDescription viewBufferDesc;
			viewBufferDesc.sizeInBytes = sizeof(glm::mat4) * 2;
			viewBufferDesc.usage = aes::MemoryUsage::Default;
			viewBufferDesc.cpuAccessFlags = aes::CPUAccessFlagBits::Write;
			viewBufferDesc.bindFlags = aes::BindFlagBits::UniformBuffer;
			viewBuffer.init(viewBufferDesc);
		}
		{
			aes::BufferDescription modelBufferDesc;
			modelBufferDesc.sizeInBytes = sizeof(glm::mat4);
			modelBufferDesc.usage = aes::MemoryUsage::Dynamic;
			modelBufferDesc.cpuAccessFlags = aes::CPUAccessFlagBits::Write;
			modelBufferDesc.bindFlags = aes::BindFlagBits::UniformBuffer;
			modelBuffer.init(modelBufferDesc);
		}

		AES_LOG("buffers initialized");

		aes::VertexInputLayout vertexInputLayout[2];
		vertexInputLayout[0].parameterName = "aPosition";
		vertexInputLayout[0].semantic = aes::SemanticType::Position;
		vertexInputLayout[0].offset = 0;
		vertexInputLayout[0].format = RHIFormat::R32G32B32_Float;

		//vertexInputLayout[1].parameterName = "aTexCoord";
		//vertexInputLayout[1].semantic = aes::SemanticType::TexCoord;
		//vertexInputLayout[1].offset = sizeof(glm::vec2);
		//vertexInputLayout[1].format = RHIFormat::R32G32_Float;

		vertexInputLayout[1].parameterName = "aColor";
		vertexInputLayout[1].semantic = aes::SemanticType::Color;
		vertexInputLayout[1].offset = sizeof(glm::vec3) * 1;
		//vertexInputLayout[1].format = RHIFormat::R8G8B8A8_Uint;
		vertexInputLayout[1].format = RHIFormat::R32G32B32A32_Float;

		aes::VertexShaderDescription vertexShaderDescription;
		static auto const source_vs = aes::readFileBin("app0:assets/shaders/vita/basic3d_vs.gxp");
		vertexShaderDescription.source = source_vs.data();
		vertexShaderDescription.verticesLayout = vertexInputLayout;
		vertexShaderDescription.verticesStride = sizeof(Vertex);
		auto err = vertexShader.init(vertexShaderDescription);
		AES_LOG("vertex shader err init");

		aes::FragmentShaderDescription fragmentShaderDescription;
		static auto const source_fs = aes::readFileBin("app0:assets/shaders/vita/basic3d_fs.gxp");
		fragmentShaderDescription.source = source_fs.data();
		fragmentShaderDescription.gxpVertexProgram = vertexShader.getGxpShader();
		err = fragmentShader.init(fragmentShaderDescription);
		AES_LOG("fragment shader init");

		aes::TextureDescription textureDesc = {};
		textureDesc.format = RHIFormat::R8G8B8A8_Uint;
		textureDesc.width = 512;
		textureDesc.height = 512;
		textureDesc.mipsLevel = 0;
		textureDesc.usage = MemoryUsage::Immutable;
		textureDesc.cpuAccess = CPUAccessFlagBits::None;

		std::vector<Color> bitmap(textureDesc.width * textureDesc.height);
		buildCheckboard(bitmap, textureDesc.width, textureDesc.height, Color::Blue, Color::Green, 0.1);
		textureDesc.initialData = bitmap.data();
		texture.init(textureDesc);
		AES_LOG("texture init");

		// enable sticks
		sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG_WIDE);
	}

	float speed = 1.0f;
	glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.0f, 0.2f));

	void update(float dt) override
	{
		AES_PROFILE_FUNCTION();

		SceCtrlData ct;
		int res = sceCtrlReadBufferPositive(0, &ct, 1);
		AES_ASSERT(res >= 0);

		if (ct.buttons & SCE_CTRL_DOWN)
		{
			//camera.pos.z -= speed * dt;
			transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, speed * dt));
		}
		if (ct.buttons & SCE_CTRL_UP)
		{
			//camera.pos.z += speed * dt;
			transform = glm::translate(transform, glm::vec3(0.0f, 0.0f, -speed * dt));
		}

		if (ct.buttons & SCE_CTRL_RIGHT)
		{
			transform = glm::translate(transform, glm::vec3(speed * dt, 0.0f, 0.0f));
		}

		if (ct.buttons & SCE_CTRL_LEFT)
		{
			transform = glm::translate(transform, glm::vec3(-speed * dt, 0.0f, 0.0f));
		}

		if (ct.ly > 150)
		{
			camera.pos.x += speed;
		}
		if (ct.ly < 100)
		{
			camera.pos.x -= speed;
		}

		uint windowWidth = 960, windowHeight = 544;
		float const aspect = (float)windowWidth / (float)windowHeight;

		camera.lookAt(camera.pos + glm::vec3(0, 0, 1));
		camera.projMatrix = glm::perspectiveLH_ZO(glm::radians(45.0f), aspect, 0.01f, 100.0f);
		//aes::CameraBuffer const camBuf{ glm::transpose(mainCamera.viewMatrix), glm::transpose(mainCamera.projMatrix) };
		aes::CameraBuffer const camBuf{ mainCamera.viewMatrix, mainCamera.projMatrix };
		viewBuffer.setDataFromPOD(camBuf);
		//transform = glm::rotate(transform, speed * dt, glm::vec3(1.0f, 1.0f, 1.0f));
		modelBuffer.setDataFromPOD(transform);
	}

	void draw() override
	{
		using namespace aes;

		AES_PROFILE_FUNCTION();

		auto context = RHIRenderContext::instance();

		context.setDrawPrimitiveMode(DrawPrimitiveType::Triangles);
		context.setVertexShader(vertexShader);
		context.setFragmentShader(fragmentShader);

		context.bindVSUniformBuffer(viewBuffer, 0);
		context.bindVSUniformBuffer(modelBuffer, 1);

		context.bindVertexBuffer(vertexBuffer, sizeof(Vertex));
		context.bindIndexBuffer(indexBuffer, IndexTypeFormat::Uint16);
		context.bindFragmentTexture(texture, 0);
		context.drawIndexed(36, 0);
	}
};

int main()
{
	const char* appName = "aes engine";
	std::ofstream logFile("ux0:log/AES_log.txt");
	aes::Logger::instance().addSink(std::make_unique<aes::PsvDebugScreenSink>());
	aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>(logFile));

	Game game({
		.appName = appName
	});

	game.init();
	game.run();
	
	return 0;
}
