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
		//uint32_t color;
		glm::vec4 color;
	};

	void start() override
	{
		using namespace aes;

		AES_PROFILE_FUNCTION();
		AES_LOG("start");

		//Vertex basicVertices[3];
		//basicVertices[0].pos = { -0.5f, -0.5f };
		////basicVertices[0].color = aes::Color::White;
		//basicVertices[0].color = { 1.0f, 0.0f, 0.0f, 1.0f };

		//basicVertices[1].pos = { 0.5f, -0.5f };
		////basicVertices[1].color = 0xff0000ff;
		//basicVertices[1].color = { 1.0f, 0.0f, 0.0f, 1.0f };

		//basicVertices[2].pos = { -0.5f, 0.5f };
		////basicVertices[2].color = 0xfff00fff;
		//basicVertices[1].color = { 0.0f, 1.0f, 0.0f, 1.0f };

		//{
		//	aes::BufferDescription vertexBufferDescription;
		//	vertexBufferDescription.sizeInBytes = sizeof(basicVertices);
		//	vertexBufferDescription.bufferUsage = aes::BufferUsage::Dynamic;
		//	vertexBufferDescription.cpuAccessFlags = aes::CPUAccessFlagBits::Write;
		//	vertexBufferDescription.bindFlags = aes::BindFlagBits::VertexBuffer;
		//	vertexBufferDescription.initialData = basicVertices;
		//	vertexBuffer.init(vertexBufferDescription);
		//}

		//uint16_t indices[] = { 0, 1, 2, 0 };
		//{
		//	aes::BufferDescription indexBufferDescription;
		//	indexBufferDescription.sizeInBytes = sizeof(indices);
		//	indexBufferDescription.bufferUsage = aes::BufferUsage::Dynamic;
		//	indexBufferDescription.cpuAccessFlags = aes::CPUAccessFlagBits::Write;
		//	indexBufferDescription.bindFlags = aes::BindFlagBits::IndexBuffer;
		//	indexBufferDescription.initialData = indices;
		//	indexBuffer.init(indexBufferDescription);
		//}
		//AES_LOG("buffers initialized");

		//aes::VertexInputLayout vertexInputLayout[2];
		//vertexInputLayout[0].parameterName = "aPosition";
		//vertexInputLayout[0].semantic = aes::SemanticType::Position;
		//vertexInputLayout[0].offset = 0;
		//vertexInputLayout[0].format = RHIFormat::R32G32_Float;

		//vertexInputLayout[1].parameterName = "aColor";
		//vertexInputLayout[1].semantic = aes::SemanticType::Color;
		//vertexInputLayout[1].offset = sizeof(glm::vec2);
		////vertexInputLayout[1].format = RHIFormat::R8G8B8A8_Uint;
		//vertexInputLayout[1].format = RHIFormat::R32G32B32A32_Float;

		//aes::VertexShaderDescription vertexShaderDescription;
		//static auto const source_vs = aes::readFileBin("app0:assets/shaders/vita/basic2d_vs.gxp");
		//vertexShaderDescription.source = source_vs.data();
		//vertexShaderDescription.verticesLayout = vertexInputLayout;
		//vertexShaderDescription.verticesStride = sizeof(Vertex);
		//auto err = vertexShader.init(vertexShaderDescription);
		//AES_LOG("vertex shader err init");

		//aes::FragmentShaderDescription fragmentShaderDescription;
		//static auto const source_fs = aes::readFileBin("app0:assets/shaders/vita/basic2d_fs.gxp");
		//fragmentShaderDescription.source = source_fs.data();
		//fragmentShaderDescription.gxpVertexProgram = vertexShader.getGxpShader();
		//err = fragmentShader.init(fragmentShaderDescription);
		//AES_LOG("fragment shader init");

		draw2d.init();
	}

	void update(float dt) override
	{
		AES_PROFILE_FUNCTION();

		draw2d.setMatrix(glm::mat4(1.0f));
		//draw2d.setColor(aes::Color::Blue);
		//draw2d.drawLine({ {0.0f, 0.0f}, {1.0f, 0.0f} });
		//draw2d.setColor(aes::Color::Green);
		//draw2d.drawLine({ {0.0f, 0.0f}, {0.0f, 1.0f} });
		draw2d.setColor(aes::Color::Red);
		draw2d.drawFillRect({ {0.5f, 0.5f}, {-1.0f, -1.0f} });
	}

	void draw() override
	{
		using namespace aes;

		AES_PROFILE_FUNCTION();
		auto context = RHIRenderContext::instance();

		//context.setDrawPrimitiveMode(DrawPrimitiveType::Triangles);
		//context.setVertexShader(vertexShader);
		//context.setFragmentShader(fragmentShader);
		//context.bindVertexBuffer(vertexBuffer, sizeof(Vertex));
		//context.bindIndexBuffer(indexBuffer, IndexTypeFormat::Uint16);
		//context.drawIndexed(3, 0);

		draw2d.executeDrawCommands();
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
