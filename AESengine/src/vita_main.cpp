#include <iostream>
#include <fstream>
#include "core/profiler.hpp"
#include "core/debugMath.hpp"
#include "engine.hpp"
#include "core/os.hpp"
#include "core/utility.hpp"
#include "core/color.hpp"
#include "renderer/RHI/RHIRenderContext.hpp"
#include "renderer/draw2d.hpp"

#include "aini/aini.hpp"

#define SCE_DBG_ASSERT AES_ASSERT

class Game : public aes::Engine
{

public:

	aes::RHIBuffer vertexBuffer;
	aes::RHIBuffer indexBuffer;
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
		uint32_t color;
	};

	uint16_t* basicIndices = nullptr;
	void start() override
	{
		using namespace aes;

		AES_PROFILE_FUNCTION();
		AES_LOG("start");

		Vertex basicVertices[3];
		basicVertices[0].pos = { -0.5f, -0.5f };
		basicVertices[0].color = 0xffffffff;

		basicVertices[1].pos = { 0.5f, -0.5f };
		basicVertices[1].color = 0xffffffff;

		basicVertices[2].pos = { -0.5f, 0.5f };
		basicVertices[2].color = 0xffffffff;

		{
			aes::BufferDescription vertexBufferDescription;
			vertexBufferDescription.sizeInBytes = sizeof(basicVertices);
			vertexBufferDescription.bufferUsage = aes::BufferUsage::Dynamic;
			vertexBufferDescription.cpuAccessFlags = aes::CPUAccessFlagBits::Write;
			vertexBufferDescription.bindFlags = aes::BindFlagBits::VertexBuffer;
			vertexBufferDescription.initialData = basicVertices;
			vertexBuffer.init(vertexBufferDescription);
		}

		uint16_t indices[] = { 0, 1, 2 };
		{
			aes::BufferDescription indexBufferDescription;
			indexBufferDescription.sizeInBytes = sizeof(indices);
			indexBufferDescription.bufferUsage = aes::BufferUsage::Dynamic;
			indexBufferDescription.cpuAccessFlags = aes::CPUAccessFlagBits::Write;
			indexBufferDescription.bindFlags = aes::BindFlagBits::IndexBuffer;
			indexBufferDescription.initialData = indices;
			indexBuffer.init(indexBufferDescription);
		}

		aes::VertexInputLayout vertexInputLayout[2];
		vertexInputLayout[0].semantic = SemanticType::Position;
		vertexInputLayout[0].offset = 0;
		vertexInputLayout[0].format = RHIFormat::R32G32_Float;

		vertexInputLayout[1].semantic = SemanticType::Color;
		vertexInputLayout[1].offset = sizeof(glm::vec2);
		vertexInputLayout[1].format = RHIFormat::U8n;

		aes::VertexShaderDescription vertexShaderDescription;
		static auto const source_vs = aes::readFileBin("app0:assets/shaders/vita/basic2d_vs.gxp");
		vertexShaderDescription.source = source_vs.data();
		vertexShaderDescription.verticesLayout = vertexInputLayout;
		vertexShaderDescription.verticesStride = sizeof(Vertex);
		AES_LOG("vertex stride {}", vertexShaderDescription.verticesStride);
		auto err = vertexShader.init(vertexShaderDescription);
		AES_LOG("shader 1 initialized");
		
		aes::FragmentShaderDescription fragmentShaderDescription;
		static auto const source_fs = aes::readFileBin("app0:assets/shaders/vita/basic2d_fs.gxp");
		fragmentShaderDescription.source = source_fs.data();
		fragmentShaderDescription.gxpVertexProgram = vertexShader.getGxpShader();
		err = fragmentShader.init(fragmentShaderDescription);
		AES_LOG("shader 2 initialized");
	}

	void update(float dt) override
	{
		AES_PROFILE_FUNCTION();
	}

	void draw() override
	{
		using namespace aes;

		AES_PROFILE_FUNCTION();
		auto context = RHIRenderContext::instance();

		context.setDrawPrimitiveMode(DrawPrimitiveType::Triangles);
		context.setVertexShader(vertexShader);
		context.setFragmentShader(fragmentShader);
		context.bindVertexBuffer(vertexBuffer, sizeof(Vertex));
		context.bindIndexBuffer(indexBuffer, TypeFormat::Uint16);
		context.drawIndexed(3, 0);
	}
};

int main()
{
	std::string appName = "aes engine";
	//std::ofstream logFile("ux0:log/AES_log.txt");
	aes::Logger::instance().addSink(std::make_unique<aes::PsvDebugScreenSink>());
	//aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>(logFile));

	Game game({
		.appName = appName.c_str()
	});

	game.init();
	game.run();
	
	return 0;
}
