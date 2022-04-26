#include <glm/gtx/transform.hpp>
#include <iostream>
#include <fstream>
#include <random>

#include "core/allocator.hpp"
#include "core/array.hpp"
#include "core/profiler.hpp"
#include "core/debugMath.hpp"
#include "engine.hpp"
#include "renderer/model.hpp"
#include "renderer/material.hpp"
#include "core/os.hpp"
#include "core/utility.hpp"
#include "core/geometry.hpp"
#include "core/color.hpp"
#include "spatial/octree.hpp"
#include "spatial/BSPTree.hpp"
#include "core/simd.hpp"
#include "core/jobSystem.hpp"

const char pxShader[] = R"(
struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
    return input.color;
}
)";

const char vShader[] = R"(
struct VS_INPUT
{
    float4 position : POSITION;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer CameraBuffer
{
	float4x4 viewMatrix;
	float4x4 projectionMatrix;
};

cbuffer ModelBuffer
{
	float4x4 worldMatrix;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    // Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    //output.position = input.position;

    // Store the input color for the pixel shader to use.
    output.color = input.color;
    
    return output;
})";

struct LineRenderer
{
	aes::RHIBuffer vertexBuffer;
	aes::RHIBuffer indexBuffer;
	aes::Array<aes::Vertex> vertices;
	aes::Array<uint32_t> indices;
	aes::Color colorState = aes::Color::Blue;

	LineRenderer() : vertices(*aes::globalAllocator), indices(*aes::globalAllocator)
	{
	}

	void init()
	{
		AES_PROFILE_FUNCTION();
		using namespace aes;

		BufferDescription vertexBufferInfo{};
		vertexBufferInfo.bindFlags = BindFlagBits::VertexBuffer;
		vertexBufferInfo.bufferUsage = BufferUsage::Dynamic;
		vertexBufferInfo.sizeInBytes = 1_mb;
		vertexBufferInfo.cpuAccessFlags = CPUAccessFlagBits::Write;

		auto err = vertexBuffer.init(vertexBufferInfo);

		BufferDescription indexBufferInfo{};
		indexBufferInfo.bindFlags = BindFlagBits::IndexBuffer;
		indexBufferInfo.bufferUsage = BufferUsage::Dynamic;
		indexBufferInfo.sizeInBytes = 1_mb;
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
		aes::RHIRenderContext::instance().bindIndexBuffer(indexBuffer, aes::TypeFormat::Uint32);
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

static void debugDrawOctree(aes::Octree const& tree, LineRenderer& render)
{
	for (auto const& [_, node] : tree)
	{
		render.setColor(aes::Color(0, 0, 255));
		render.addAABB(aes::AABB::createHalfCenter(node.center, glm::vec3(node.halfSize)));
	}
}

class Game : public aes::Engine
{

public:

	aes::JobSystem jobSystem;
	LineRenderer lineRenderer;
	aes::RHIFragmentShader fragmentShader;
	aes::RHIVertexShader vertexShader;
	aes::RHIBuffer viewBuffer;
	aes::RHIBuffer identityModelBuffer;
	aes::Material defaultMtrl;
	aes::Model model;
	
	TestElement testElements[25];
	aes::Octree octree;
	aes::UniquePtr<aes::BSPTree::BSPElement> bspTree;
	
	Game(InitInfo const& info) : Engine(info), jobSystem(*aes::globalAllocator)
	{
		AES_LOG("Game initialized");
		auto vert = aes::getCubeVertices();
		aes::parrallelForEach(jobSystem, vert, [](auto& v)
			{
				AES_LOG("{}", v.pos);
			});
	}

	void start() override
	{
		using namespace aes;

		AES_PROFILE_FUNCTION();
		AES_LOG("start");

		aes::FragmentShaderDescription fragmentShaderDescription;
		fragmentShaderDescription.source = pxShader;

		if (!fragmentShader.init(fragmentShaderDescription))
		{
			AES_ASSERTF(false, "fragment shader creation failed");
		}

		AES_LOG("fragment shader created");

		aes::VertexShaderDescription vertexShaderDescription;

		vertexShaderDescription.source = vShader;
		vertexShaderDescription.verticesStride = sizeof(aes::Vertex);

		aes::VertexInputLayout vertexInputLayout[2];
		vertexInputLayout[0].semantic = aes::SemanticType::Position;
		vertexInputLayout[0].offset = 0;
		vertexInputLayout[0].format = aes::RHIFormat::R32G32B32_Float;

		vertexInputLayout[1].semantic = aes::SemanticType::Color;
		vertexInputLayout[1].offset = sizeof(glm::vec3);
		vertexInputLayout[1].format = aes::RHIFormat::R32G32B32A32_Float;

		vertexShaderDescription.verticesLayout = vertexInputLayout;

		if (!vertexShader.init(vertexShaderDescription))
		{
			AES_ASSERTF(false, "vertex shader creation failed");
		}
		AES_LOG("vertex shader created");

		if (!defaultMtrl.init(&vertexShader, &fragmentShader))
		{
			AES_ASSERTF(false, "material creation failed");
		}
		AES_LOG("material created");

		// generate test elements
		std::random_device rd;  // Will be used to obtain a seed for the random number engine
		std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
		std::uniform_real_distribution const dis(-10.0, 10.0);
		std::uniform_real_distribution const disS(1.0, 2.0);
		lineRenderer.setColor(aes::Color::Blue);
		auto cb = [](void* userData)
		{
			TestElement* element = static_cast<TestElement*>(userData);
			element->coll = true;
		};
#define USE_BSP
#ifdef USE_BSP
		static aes::StackAllocator stackAlloc(mallocator, 16_mb);
		//globalAllocator = &stackAlloc;
		aes::Array<aes::BSPTree::Object> bspObjects(*globalAllocator);
		bspObjects.reserve(std::size(testElements));
		for (int i = 0; auto & e : testElements)
		{
			e.id = i++;
			e.pos = glm::vec3(dis(gen), dis(gen), dis(gen));
			e.size = glm::vec3(disS(gen), disS(gen), disS(gen));
			bspObjects.push({ &e, aes::AABB::createHalfCenter(e.pos, e.size) });
		}
		
		bspTree = aes::BSPTree::build(*globalAllocator, std::span(bspObjects));
		bspTree->testAllCollisions(cb);
#endif
		// octree ========
#ifdef USE_OCTREE
		//for (int a = 0; a < 10'000; a++)
		{
			octree.clear();
			octree.build(glm::vec3(0), 20.0f, 1);
			for (int i = 0; auto & e : testElements)
			{
				e.id = i++;
				e.pos = glm::vec3(dis(gen), dis(gen), dis(gen));
				e.size = glm::vec3(disS(gen), disS(gen), disS(gen));
				octree.insertObject(*octree.root(), { aes::AABB::createHalfCenter(e.pos, e.size), &e });
			}
			//for (auto const& [c, n] : octree)
			//{
			//	AES_LOG("code : {}, depth : {}, num objects : {}", c, aes::Octree::getNodeTreeDepth(n), n.objects.size());
			//}
			//debugDrawOctree(octree, lineRenderer);

			octree.testAllCollisions(*octree.root(), cb);
		}
#endif
		// octree ===========

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
		lineRenderer.addLine(glm::vec3(0, 0, 0), glm::vec3(10, 0, 0));
		lineRenderer.setColor(aes::Color::Green);
		lineRenderer.addLine(glm::vec3(0, 0, 0), glm::vec3(0, 10, 0));
		lineRenderer.setColor(aes::Color::Red);
		lineRenderer.addLine(glm::vec3(0, 0, 0), glm::vec3(0, 0, 10));

		{
			aes::BufferDescription viewDesc;
			viewDesc.bindFlags = aes::BindFlagBits::UniformBuffer;
			viewDesc.bufferUsage = aes::BufferUsage::Dynamic;
			viewDesc.cpuAccessFlags = aes::CPUAccessFlagBits::Write;
			viewDesc.sizeInBytes = sizeof(aes::CameraBuffer);
			viewBuffer.init(viewDesc);
			AES_LOG("view buffer created");
		}
		{
			aes::BufferDescription identityDesc;
			identityDesc.bindFlags = aes::BindFlagBits::UniformBuffer;
			identityDesc.bufferUsage = aes::BufferUsage::Immutable;
			identityDesc.cpuAccessFlags = aes::CPUAccessFlagBits::None;
			identityDesc.sizeInBytes = sizeof(glm::mat4);
			glm::mat4 identity(1.0f);
			identityDesc.initialData = (void*)&identity;
			identityModelBuffer.init(identityDesc);
			AES_LOG("identity buffer created");
		}
		mainCamera.pos = { 0.0, 0.0, -5.0 };
		getViewportMousePos(lastMousePosX, lastMousePosY);
	}

	uint drawLevel = 0;
	float speed = 6.0f, sensitivity = 60.f;
	float lastMousePosX, lastMousePosY;
	glm::vec3 direction = {0.0, 0.0, 1.0};
	float yaw = 91, pitch = 2;

	void update(float dt) override
	{
		AES_PROFILE_FUNCTION();
		
		glm::vec4 movePos = { 0.0f, 0.f, 0.f, 0.0f };

		if (getKeyState(aes::Key::W) == aes::InputState::Down)
		{
			movePos.z += speed * dt;
		}

		if (getKeyState(aes::Key::S) == aes::InputState::Down)
		{
			movePos.z -= speed * dt;
		}

		if (getKeyState(aes::Key::D) == aes::InputState::Down)
		{
			movePos.x += speed * dt;
		}

		if (getKeyState(aes::Key::A) == aes::InputState::Down)
		{
			movePos.x -= speed * dt;
		}

		if (getKeyState(aes::Key::E) == aes::InputState::Down)
		{
			movePos.y += speed * dt;
		}

		if (getKeyState(aes::Key::Q) == aes::InputState::Down)
		{
			movePos.y -= speed * dt;
		}

		mainCamera.pos += glm::vec3(movePos * mainCamera.viewMatrix);
		float mx, my;
		getViewportMousePos(mx, my);
		
		if (getKeyState(aes::Key::RClick) == aes::InputState::Down)
		{
			float xoffset = mx - lastMousePosX;
			float yoffset = my - lastMousePosY; // reversed since y-coordinates range from bottom to top

			lastMousePosX = mx;
			lastMousePosY = my;

			xoffset *= sensitivity;
			yoffset *= sensitivity;

			yaw += xoffset;
			pitch += yoffset;

			if (pitch > 89.0f)
				pitch = 89.0f;
			if (pitch < -89.0f)
				pitch = -89.0f;

			direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
			direction.y = sin(glm::radians(pitch));
			direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		}
		else
		{
			getViewportMousePos(lastMousePosX, lastMousePosY);
		}
		mainCamera.lookAt(mainCamera.pos + glm::normalize(direction));
		TestElement* elem = (TestElement*)bspTree->raycast({ mainCamera.pos, glm::normalize(direction) });
		if (elem)
		{
			elem->coll = true;
		}
		
		{
			float const ex = 0.0055f;
			float const csx = 0.025f;

			uint windowWidth = 960, windowHeight = 544;
			mainWindow->getScreenSize(windowWidth, windowHeight);
			float const aspect = (float)windowWidth / (float)windowHeight;
			mainCamera.projMatrix = glm::perspectiveLH_ZO(glm::radians(45.0f), aspect, 0.1f, 1000.0f);
		}
		aes::CameraBuffer const camBuf{ glm::transpose(mainCamera.viewMatrix), glm::transpose(mainCamera.projMatrix) };
		viewBuffer.setDataFromPOD(camBuf);
	}

	void draw() override
	{
		AES_PROFILE_FUNCTION();
		
		aes::Material::BindInfo bindInfo;

		for (auto& e : testElements)
		{

			bindInfo.vsBuffers.push_back(std::make_pair("ModelBuffer", &e.model.modelBuffer));
			bindInfo.vsBuffers.push_back(std::make_pair("CameraBuffer", &viewBuffer));
			defaultMtrl.bind(bindInfo);

			e.model.toWorld = glm::translate(glm::mat4(1.0f), e.pos);
			e.model.toWorld = glm::scale(e.model.toWorld, e.size);
			e.model.draw();
		}
		
		aes::RHIRenderContext::instance().bindVSUniformBuffer(identityModelBuffer, 1);
		lineRenderer.draw();
	}
};

int main()
{
	std::ofstream logFile("AES_log.txt");
	aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>(std::cout));
	//aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>(logFile));
	
	AES_START_PROFILE_SESSION("startup");
	Game game({
		.appName = "aes engine"
	});

	game.init();
	auto startupSession = AES_STOP_PROFILE_SESSION();
	
	AES_START_PROFILE_SESSION("running");
	game.run();
	auto runningSession = AES_STOP_PROFILE_SESSION();
	
	std::ofstream timmingFile("prof.txt");

	for (auto const& [_, v] : runningSession.profileDatas)
	{
		if (strstr(v.name, "BSPTree"))
		{
			timmingFile << fmt::format("{}\n\ttotalTime: {}ms\n\tcount: {}\n\taverage: {}ms\n\tparent: {}\n",
				v.name, v.elapsedTime, v.count, v.elapsedTime / v.count, v.parentName != nullptr ? v.parentName : "none");
		}
	}
	//
	return 0;
}
