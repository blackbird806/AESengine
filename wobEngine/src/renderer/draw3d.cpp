#include "draw3d.hpp"
#include "RHI/RHIDevice.hpp"

#include "RHI/inlineShaders/draw3dShader.hpp"

using namespace wob;

void Draw3D::init(RHIDevice& device_)
{
	WOB_PROFILE_FUNCTION();

	device = &device_;
	pipeline.init(device);

	VertexShaderDescription vertexShaderDesc;
	vertexShaderDesc.sourceCode = draw3dVSSource;
	vertexShaderDesc.verticesLayout.resize(2);

	vertexShaderDesc.verticesLayout[0].semantic = SemanticType::Position;
	vertexShaderDesc.verticesLayout[0].parameterName = "position";
	vertexShaderDesc.verticesLayout[0].offset = 0;
	vertexShaderDesc.verticesLayout[0].format = RHIFormat::R32G32B32_Float;
	vertexShaderDesc.verticesLayout[0].vertexBufferIndex = 0;
	vertexShaderDesc.verticesLayout[0].classification = VertexInputClassification::PerVertex;

	vertexShaderDesc.verticesLayout[1].semantic = SemanticType::Color;
	vertexShaderDesc.verticesLayout[1].parameterName = "color";
	vertexShaderDesc.verticesLayout[1].offset = sizeof(vec3);
	vertexShaderDesc.verticesLayout[1].format = RHIFormat::R32G32B32A32_Float;
	vertexShaderDesc.verticesLayout[1].vertexBufferIndex = 0;
	vertexShaderDesc.verticesLayout[1].classification = VertexInputClassification::PerVertex;

	vertexShaderDesc.verticesStride = sizeof(Vertex);

	pipeline.buildVertexShader(vertexShaderDesc);

	BlendInfo blendInfo{};
	blendInfo.colorMask = ColorMaskBits::All;
	blendInfo.alphaOp = BlendOp::Add;
	blendInfo.colorOp = BlendOp::Add;
	blendInfo.alphaDst = BlendFactor::OneMinusDstAlpha;
	blendInfo.alphaSrc = BlendFactor::One;
	blendInfo.colorDst = BlendFactor::OneMinusSrcAlpha;
	blendInfo.colorSrc = BlendFactor::SrcAlpha;

	FragmentShaderDescription fragmentShaderDesc;
	fragmentShaderDesc.sourceCode = draw3dFSSource;
	fragmentShaderDesc.blendInfo = &blendInfo;
	fragmentShaderDesc.multisampleMode = MultisampleMode::None;
	//fragmentShaderDesc.gxpVertexProgram
	pipeline.buildFragmentShader(fragmentShaderDesc);

	BufferDescription vertexBufferDesc{};
	vertexBufferDesc.bindFlags = BindFlagBits::VertexBuffer;
	vertexBufferDesc.usage = MemoryUsage::Dynamic;
	vertexBufferDesc.cpuAccessFlags = CPUAccessFlagBits::Write;
	vertexBufferDesc.sizeInBytes = vertexBufferSize;
	vertexBuffer = device->createBuffer(vertexBufferDesc).value();

	BufferDescription indexBufferDesc{};
	indexBufferDesc.bindFlags = BindFlagBits::IndexBuffer;
	indexBufferDesc.usage = MemoryUsage::Dynamic;
	indexBufferDesc.cpuAccessFlags = CPUAccessFlagBits::Write;
	indexBufferDesc.sizeInBytes = vertexBufferSize;
	indexBuffer = device->createBuffer(indexBufferDesc).value();
}

void Draw3D::setColor(Color col)
{
	currentState.color = col;
}

void Draw3D::drawWireAABB(AABB aabb)
{
	vec3 const pos = aabb.center();
	vec3 const halfSize = aabb.halfSize();
	{
		Line3D l1, l2, l3;
		l1.p1 = pos - halfSize;
		l1.p2 = l1.p1;
		l1.p2.z = pos.z + halfSize.z;

		l2.p1 = pos - halfSize;
		l2.p2 = l2.p1;
		l2.p2.y = pos.y + halfSize.y;

		l3.p1 = pos - halfSize;
		l3.p2 = l3.p1;
		l3.p2.x = pos.x + halfSize.x;

		drawLine(l1);
		drawLine(l2);
		drawLine(l3);
	}
	{
		Line3D l1, l2, l3;
		l1.p1 = pos + halfSize;
		l1.p2 = l1.p1;
		l1.p2.z = pos.z - halfSize.z;

		l2.p1 = pos + halfSize;
		l2.p2 = l2.p1;
		l2.p2.y = pos.y - halfSize.y;

		l3.p1 = pos + halfSize;
		l3.p2 = l3.p1;
		l3.p2.x = pos.x - halfSize.x;

		drawLine(l1);
		drawLine(l2);
		drawLine(l3);
	}
	{
		Line3D l1, l2, l3;
		l1.p1 = pos + halfSize;
		l1.p1.z = pos.z - halfSize.z;
		l1.p2 = l1.p1;
		l1.p2.x = pos.x - halfSize.x;

		l2.p1 = pos + halfSize;
		l2.p1.x = pos.x - halfSize.x;
		l2.p2 = l2.p1;
		l2.p2.y = pos.y - halfSize.y;

		l3.p1 = pos + halfSize;
		l3.p1.x = pos.x - halfSize.x;
		l3.p2 = l3.p1;
		l3.p2.z = pos.z - halfSize.z;

		drawLine(l1);
		drawLine(l2);
		drawLine(l3);
	}
	{
		Line3D l1, l2, l3;
		l1.p1 = pos + halfSize;
		l1.p1.y = pos.y - halfSize.y;
		l1.p2 = l1.p1;
		l1.p2.x = pos.x - halfSize.x;

		l2.p1 = pos + halfSize;
		l2.p1.y = pos.y - halfSize.y;
		l2.p2 = l2.p1;
		l2.p2.z = pos.z - halfSize.z;

		l3.p1.x = pos.x + halfSize.x;
		l3.p1.y = pos.y - halfSize.y;
		l3.p1.z = pos.z - halfSize.z;
		l3.p2 = l3.p1;
		l3.p2.y = pos.y + halfSize.y;

		drawLine(l1);
		drawLine(l2);
		drawLine(l3);
	}
}

void Draw3D::drawFillAABB(AABB aabb)
{
	WOB_NOT_IMPLEMENTED();
}

void Draw3D::drawLine(Line3D line)
{
	WOB_PROFILE_FUNCTION();
	commands.push(Command{ DrawType::Line, currentState });
	vec4 const color = currentState.color.toVec4();
	vertices.push({ line.p1, color });
	vertices.push({ line.p2, color });

	indices.push(indicesOffset);
	indices.push(indicesOffset + 1);
	indicesOffset += 2;
}

void Draw3D::executeDrawCommands()
{
	WOB_PROFILE_FUNCTION();
	
	indicesOffset = 0;

	device->setBufferData(vertexBuffer, vertices.data(), vertices.size() * sizeof(Vertex));
	device->setBufferData(indexBuffer, indices.data(), indices.size() * sizeof(uint16_t));

	device->bindVertexBuffer(vertexBuffer, 0, pipeline.getVertexShaderDesc().verticesStride);
	device->bindIndexBuffer(indexBuffer, IndexTypeFormat::Uint16);
	pipeline.bind();

	uint indicesCount = 0;
	uint indicesOffset = 0;

	for (auto const& cmd : commands)
	{
		if (cmd.type == DrawType::Line)
		{
			device->setDrawPrimitiveMode(DrawPrimitiveType::Lines);
			indicesCount = 2;
		}
		device->drawIndexed(indicesCount, indicesOffset);
		indicesOffset += indicesCount;
	}
	
	indices.clear();
	vertices.clear();
	commands.clear();
}



