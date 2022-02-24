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
#include "renderer/RHI/RHITexture.hpp"
#include "renderer/textureUtility.hpp"

class Game : public aes::Engine
{

public:

	aes::RHIBuffer vertexBuffer;
	aes::RHIBuffer indexBuffer;
	aes::RHIBuffer uniformBuffer;
	aes::RHITexture texture;
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
		glm::vec2 uv;
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
		//basicVertices[0].uv = { 0.0f, 0.0f };
		//basicVertices[0].color = { 1.0f, 0.0f, 1.0f, 1.0f };

		//basicVertices[1].pos = { 0.5f, -0.5f };
		//basicVertices[1].uv = { 1.0f, 0.0f };
		////basicVertices[1].color = 0xff0000ff;
		//basicVertices[1].color = { 1.0f, 1.0f, 1.0f, 1.0f };

		//basicVertices[2].pos = { -0.5f, 0.5f };
		////basicVertices[2].color = 0xfff00fff;
		//basicVertices[2].uv = { 0.0f, 1.0f };
		//basicVertices[2].color = { 1.0f, 1.0f, 1.0f, 1.0f };

		//{
		//	aes::BufferDescription vertexBufferDescription;
		//	vertexBufferDescription.sizeInBytes = sizeof(basicVertices);
		//	vertexBufferDescription.usage = aes::MemoryUsage::Dynamic;
		//	vertexBufferDescription.cpuAccessFlags = aes::CPUAccessFlagBits::Write;
		//	vertexBufferDescription.bindFlags = aes::BindFlagBits::VertexBuffer;
		//	vertexBufferDescription.initialData = basicVertices;
		//	vertexBuffer.init(vertexBufferDescription);
		//}

		//uint16_t indices[] = { 0, 1, 2, 0 };
		//{
		//	aes::BufferDescription indexBufferDescription;
		//	indexBufferDescription.sizeInBytes = sizeof(indices);
		//	indexBufferDescription.usage = aes::MemoryUsage::Dynamic;
		//	indexBufferDescription.cpuAccessFlags = aes::CPUAccessFlagBits::Write;
		//	indexBufferDescription.bindFlags = aes::BindFlagBits::IndexBuffer;
		//	indexBufferDescription.initialData = indices;
		//	indexBuffer.init(indexBufferDescription);
		//}

		//{
		//	aes::BufferDescription uniformBufferDesc;
		//	uniformBufferDesc.sizeInBytes = sizeof(float) * 16;
		//	uniformBufferDesc.usage = aes::MemoryUsage::Dynamic;
		//	uniformBufferDesc.cpuAccessFlags = aes::CPUAccessFlagBits::Write;
		//	uniformBufferDesc.bindFlags = aes::BindFlagBits::UniformBuffer;
		//	uniformBuffer.init(uniformBufferDesc);
		//}
		//AES_LOG("buffers initialized");

		//aes::VertexInputLayout vertexInputLayout[3];
		//vertexInputLayout[0].parameterName = "aPosition";
		//vertexInputLayout[0].semantic = aes::SemanticType::Position;
		//vertexInputLayout[0].offset = 0;
		//vertexInputLayout[0].format = RHIFormat::R32G32_Float;

		//vertexInputLayout[1].parameterName = "aTexCoord";
		//vertexInputLayout[1].semantic = aes::SemanticType::TexCoord;
		//vertexInputLayout[1].offset = sizeof(glm::vec2);
		//vertexInputLayout[1].format = RHIFormat::R32G32_Float;

		//vertexInputLayout[2].parameterName = "aColor";
		//vertexInputLayout[2].semantic = aes::SemanticType::Color;
		//vertexInputLayout[2].offset = sizeof(glm::vec2) * 2;
		////vertexInputLayout[2].format = RHIFormat::R8G8B8A8_Uint;
		//vertexInputLayout[2].format = RHIFormat::R32G32B32A32_Float;

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

		//aes::TextureDescription textureDesc = {};
		//textureDesc.format = RHIFormat::R8G8B8A8_Uint;
		//textureDesc.width = 512;
		//textureDesc.height = 512;
		//textureDesc.mipsLevel = 0;
		//textureDesc.usage = MemoryUsage::Immutable;
		//textureDesc.cpuAccess = CPUAccessFlagBits::None;

		//std::vector<Color> bitmap(textureDesc.width * textureDesc.height);
		//buildCheckboard(bitmap, textureDesc.width, textureDesc.height, Color::Blue, Color::Green, 0.1);
		//textureDesc.initialData = bitmap.data();
		//texture.init(textureDesc);
		//AES_LOG("texture init");

		draw2d.init();
	}

	float angle = 0.0f;
	float speed = 5.0f;
	float wvpData[16];

	void update(float dt) override
	{
		AES_PROFILE_FUNCTION();

		//SceCtrlData ct[6];
		//int res = sceCtrlReadBufferPositive(0, ct, 6);
		//AES_ASSERT(res >= 0);
		//int btn = 0;
		//for (int i = 0; i < res; i++) {
		//	btn |= ct[i].buttons;
		//}

		//if (btn & SCE_CTRL_L)
		//{
		//	angle -= speed * dt;
		//}

		//if (btn & SCE_CTRL_R)
		//{
		//	angle += speed * dt;
		//}

		//float aspectRatio = (float)960 / (float)540;

		//float s = sin(angle);
		//float c = cos(angle);

		//wvpData[0] = c / aspectRatio;
		//wvpData[1] = s;
		//wvpData[2] = 0.0f;
		//wvpData[3] = 0.0f;

		//wvpData[4] = -s / aspectRatio;
		//wvpData[5] = c;
		//wvpData[6] = 0.0f;
		//wvpData[7] = 0.0f;

		//wvpData[8] = 0.0f;
		//wvpData[9] = 0.0f;
		//wvpData[10] = 1.0f;
		//wvpData[11] = 0.0f;

		//wvpData[12] = 0.0f;
		//wvpData[13] = 0.0f;
		//wvpData[14] = 0.0f;
		//wvpData[15] = 1.0f;
		draw2d.setMatrix(glm::mat4(1.0f));
		draw2d.setColor(aes::Color::Blue);
		draw2d.drawLine({ {0.0f, 0.2f}, {1.0f, 0.0f} });
		draw2d.setColor(aes::Color::Green);
		draw2d.drawLine({ {0.0f, 0.0f}, {0.0f, 1.0f} });
		draw2d.setColor(aes::Color::Red);
		draw2d.drawFillRect({ {0.5f, 0.5f}, {0.6f, 0.6f} });
	}

	void draw() override
	{
		using namespace aes;

		AES_PROFILE_FUNCTION();
		auto context = RHIRenderContext::instance();


		//context.setDrawPrimitiveMode(DrawPrimitiveType::Triangles);
		//context.setVertexShader(vertexShader);
		//context.setFragmentShader(fragmentShader);

		//uniformBuffer.setData(wvpData, sizeof(float) * 16);
		//context.bindVSUniformBuffer(uniformBuffer, 0);

		//context.bindVertexBuffer(vertexBuffer, sizeof(Vertex));
		//context.bindIndexBuffer(indexBuffer, IndexTypeFormat::Uint16);
		//context.bindFragmentTexture(texture, 0);
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
