#include <iostream>
#include <fstream>
#include <span>
#include <random>
#include <glm/gtx/transform.hpp>
#include "core/profiler.hpp"
#include "core/debugMath.hpp"
#include "engine.hpp"
#include "core/os.hpp"
#include "core/utility.hpp"
#include "core/color.hpp"
#include "core/array.hpp"
#include "core/uniquePtr.hpp"
#include "renderer/RHI/RHIRenderContext.hpp"
#include "renderer/draw2d.hpp"
#include "renderer/RHI/RHITexture.hpp"
#include "renderer/textureUtility.hpp"
#include "renderer/model.hpp"
#include "renderer/camera.hpp"
#include "spatial/BSPTree.hpp"
#include "core/geometry.hpp"

struct LineRenderer
{
	aes::RHIBuffer vertexBuffer;
	aes::RHIBuffer indexBuffer;
	aes::Array<aes::Vertex> vertices;
	aes::Array<uint32_t> indices;
	aes::Color colorState = aes::Color::Blue;

	LineRenderer() : vertices(aes::globalAllocator), indices(aes::globalAllocator)
	{
	}

	void init()
	{
		AES_PROFILE_FUNCTION();
		using namespace aes;

		BufferDescription vertexBufferInfo{};
		vertexBufferInfo.bindFlags = BindFlagBits::VertexBuffer;
		vertexBufferInfo.sizeInBytes = 2_mb;
		vertexBufferInfo.usage = MemoryUsage::Dynamic;
		vertexBufferInfo.cpuAccessFlags = CPUAccessFlagBits::Write;

		auto err = vertexBuffer.init(vertexBufferInfo);

		BufferDescription indexBufferInfo{};
		indexBufferInfo.bindFlags = BindFlagBits::IndexBuffer;
		indexBufferInfo.sizeInBytes = 1_mb;
		indexBufferInfo.usage = MemoryUsage::Dynamic;
		indexBufferInfo.cpuAccessFlags = CPUAccessFlagBits::Write;

		err = indexBuffer.init(indexBufferInfo);
	}

	void setColor(aes::Color color)
	{
		colorState = color;
	}

	void addLine(glm::vec3 const& from, glm::vec3 const& to)
	{
		AES_PROFILE_FUNCTION();

		glm::vec4 const vcolor = colorState.toVec4();
		vertices.push({ from, vcolor });
		vertices.push({ to, vcolor });
	}

	void addAABB(aes::AABB const& aabb)
	{
		AES_PROFILE_FUNCTION();
		addLine(glm::vec3(aabb.min), glm::vec3(aabb.min.x, aabb.min.y, aabb.max.z));
		addLine(glm::vec3(aabb.min), glm::vec3(aabb.min.x, aabb.max.y, aabb.min.z));
		addLine(glm::vec3(aabb.min), glm::vec3(aabb.max.x, aabb.min.y, aabb.min.z));

		addLine(glm::vec3(aabb.max), glm::vec3(aabb.min.x, aabb.max.y, aabb.max.z));
		addLine(glm::vec3(aabb.max), glm::vec3(aabb.max.x, aabb.min.y, aabb.max.z));
		addLine(glm::vec3(aabb.max), glm::vec3(aabb.max.x, aabb.max.y, aabb.min.z));

		addLine(glm::vec3(aabb.min.x, aabb.max.y, aabb.min.z), glm::vec3(aabb.max.x, aabb.max.y, aabb.min.z));
		addLine(glm::vec3(aabb.max.x, aabb.min.y, aabb.min.z), glm::vec3(aabb.max.x, aabb.max.y, aabb.min.z));
		addLine(glm::vec3(aabb.max.x, aabb.min.y, aabb.max.z), glm::vec3(aabb.max.x, aabb.min.y, aabb.min.z));

		addLine(glm::vec3(aabb.min.x, aabb.max.y, aabb.max.z), glm::vec3(aabb.min.x, aabb.max.y, aabb.min.z));
		addLine(glm::vec3(aabb.min.x, aabb.min.y, aabb.max.z), glm::vec3(aabb.min.x, aabb.max.y, aabb.max.z));
		addLine(glm::vec3(aabb.min.x, aabb.min.y, aabb.max.z), glm::vec3(aabb.max.x, aabb.min.y, aabb.max.z));
	}

	void clear()
	{
		vertices.clear();
	}

	void draw()
	{
		AES_PROFILE_FUNCTION();
		indices.resize(vertices.size());
		for (uint32_t i = 0; i < vertices.size(); i++)
			indices[i] = i;

		vertexBuffer.setData(std::span(vertices));
		indexBuffer.setData(std::span(indices));
		aes::RHIRenderContext::instance().bindVertexBuffer(vertexBuffer, sizeof(aes::Vertex));
		aes::RHIRenderContext::instance().bindIndexBuffer(indexBuffer, aes::IndexTypeFormat::Uint32);
		aes::RHIRenderContext::instance().setDrawPrimitiveMode(aes::DrawPrimitiveType::Lines);
		aes::RHIRenderContext::instance().drawIndexed(indices.size());
	}

};

struct TestElement
{
	int id;
	glm::vec3 pos;
	glm::vec3 size;
	aes::Model model;
	bool coll = false;
};

class Game : public aes::Engine
{

public:

	aes::RHIBuffer viewBuffer, modelBuffer;
	aes::RHITexture texture;
	aes::RHIFragmentShader fragmentShader;
	aes::RHIVertexShader vertexShader;
	TestElement testElements[25];
	LineRenderer lineRenderer;

	aes::UniquePtr<aes::BSPTree::BSPElement> bspTree;

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
			aes::BufferDescription viewBufferDesc;
			viewBufferDesc.sizeInBytes = sizeof(CameraBuffer);
			viewBufferDesc.usage = aes::MemoryUsage::Dynamic;
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
		AES_LOG("vertex shader init");

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

		// generate test elements
		std::random_device rd;  // Will be used to obtain a seed for the random number engine
		std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
		std::uniform_real_distribution dis(-10.0f, 10.0f);
		std::uniform_real_distribution disS(1.0f, 2.0f);
		lineRenderer.setColor(aes::Color::Blue);
		auto cb = [](void* userData)
		{
			TestElement* element = static_cast<TestElement*>(userData);
			element->coll = true;
		};
#define USE_BSP
#ifdef USE_BSP
		aes::Array<aes::BSPTree::Object> bspObjects(globalAllocator);
		bspObjects.reserve(std::size(testElements));
		for (int i = 0; auto & e : testElements)
		{
			e.id = i++;
			e.pos = glm::vec3(dis(gen), dis(gen), dis(gen));
			e.size = glm::vec3(disS(gen), disS(gen), disS(gen));
			bspObjects.push({ &e, aes::AABB::createHalfCenter(e.pos, e.size) });
		}

		bspTree = aes::BSPTree::build(globalAllocator, std::span(bspObjects));
		bspTree->testAllCollisions(cb);
#endif

		// create a minor color difference in order to distinct objects
		float decR = 1.0f, decG = 1.0f;
		for (auto& e : testElements)
		{
			if (e.coll)
			{
				e.model = aes::createCube({ decR, 0.0, 0.0, 1.0 }).value();
				decR -= 0.05f;
			}
			else
			{
				e.model = aes::createCube({ 0.0, decG, 0.0, 1.0 }).value();
				decG -= 0.025f;
			}
		}
		AES_LOG("cubes created");
		lineRenderer.init();

		// draw center transform
		lineRenderer.setColor(aes::Color::Blue);
		lineRenderer.addLine(glm::vec3(0, 0, 0), glm::vec3(10, 0, 0));
		//lineRenderer.setColor(aes::Color::Green);
		//lineRenderer.addLine(glm::vec3(0, 0, 0), glm::vec3(0, 10, 0));
		//lineRenderer.setColor(aes::Color::Red);
		//lineRenderer.addLine(glm::vec3(0, 0, 0), glm::vec3(0, 0, 10));
	}

	float speed = 2.0f;

	void update(float dt) override
	{
		AES_PROFILE_FUNCTION();

		SceCtrlData ct;
		int res = sceCtrlReadBufferPositive(0, &ct, 1);
		AES_ASSERT(res >= 0);

		if (ct.ly > 150)
		{
			mainCamera.pos.z += speed * dt;
		}
		if (ct.ly < 100)
		{
			mainCamera.pos.z -= speed * dt;
		}

		if (ct.lx > 150)
		{
			mainCamera.pos.x += speed * dt;
		}
		if (ct.lx < 100)
		{
			mainCamera.pos.x -= speed * dt;
		}

		if (ct.ry > 150)
		{
			mainCamera.pos.y += speed * dt;
		}
		if (ct.ry < 100)
		{
			mainCamera.pos.y -= speed * dt;
		}

		uint windowWidth = 960, windowHeight = 544;
		float const aspect = (float)windowWidth / (float)windowHeight;

		mainCamera.viewMatrix = glm::lookAtLH(mainCamera.pos, mainCamera.pos + glm::vec3(0, 0, 1), { 0.0f, 1.0f, 0.0f });
		mainCamera.projMatrix = glm::perspectiveLH_ZO(glm::radians(45.0f), aspect, 0.01f, 100.0f);
		aes::CameraBuffer const camBuf{ mainCamera.viewMatrix, mainCamera.projMatrix };
		viewBuffer.setDataFromPOD(camBuf);
		modelBuffer.setDataFromPOD(glm::mat4(1.0f));
	}

	void draw() override
	{
		using namespace aes;

		AES_PROFILE_FUNCTION();

		auto context = RHIRenderContext::instance();

		//context.setDrawPrimitiveMode(DrawPrimitiveType::Triangles);
		context.setVertexShader(vertexShader);
		context.setFragmentShader(fragmentShader);

		context.bindVSUniformBuffer(viewBuffer, 0);
		context.bindVSUniformBuffer(modelBuffer, 1);
		lineRenderer.draw();

		//for (auto& e : testElements)
		//{

		//	//context.bindVertexBuffer(vertexBuffer, sizeof(Vertex));
		//	//context.bindIndexBuffer(indexBuffer, IndexTypeFormat::Uint16);
		//	////context.bindFragmentTexture(texture, 0);
		//	//context.drawIndexed(36, 0);
		//	e.model.toWorld = glm::translate(glm::mat4(1.0f), e.pos);
		//	e.model.toWorld = glm::scale(e.model.toWorld, e.size);
		//	//e.model.draw();
		//}
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
