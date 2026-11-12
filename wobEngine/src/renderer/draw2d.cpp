#include "draw2d.hpp"

#include "core/maths.hpp"
#include "core/utility.hpp"
#include "fontRenderer.hpp"
#include "RHI/inlineShaders/draw2dShader.hpp"

using namespace wob;

Result<void> Draw2D::init(RHIDevice& dev)
{
	WOB_PROFILE_FUNCTION();

	device = &dev;

	// init shaders
	{
		VertexShaderDescription vertexShaderDescription;
		vertexShaderDescription.verticesLayout.resize(3);

		vertexShaderDescription.verticesLayout[0].semantic = SemanticType::Position;
		vertexShaderDescription.verticesLayout[0].parameterName = "position";
		vertexShaderDescription.verticesLayout[0].offset = 0;
		vertexShaderDescription.verticesLayout[0].format = RHIFormat::R32G32_Float;
		vertexShaderDescription.verticesLayout[0].vertexBufferIndex = 0;
		vertexShaderDescription.verticesLayout[0].classification = VertexInputClassification::PerVertex;

		vertexShaderDescription.verticesLayout[1].semantic = SemanticType::Color;
		vertexShaderDescription.verticesLayout[1].parameterName = "color";
		vertexShaderDescription.verticesLayout[1].offset = sizeof(vec2);
		vertexShaderDescription.verticesLayout[1].format = RHIFormat::R32G32B32A32_Float;
		vertexShaderDescription.verticesLayout[1].vertexBufferIndex = 0;
		vertexShaderDescription.verticesLayout[1].classification = VertexInputClassification::PerVertex;

		vertexShaderDescription.verticesLayout[2].semantic = SemanticType::TexCoord;
		vertexShaderDescription.verticesLayout[2].parameterName = "uv";
		vertexShaderDescription.verticesLayout[2].offset = sizeof(vec2) + sizeof(vec4);
		vertexShaderDescription.verticesLayout[2].format = RHIFormat::R32G32_Float;
		vertexShaderDescription.verticesLayout[2].vertexBufferIndex = 0;
		vertexShaderDescription.verticesLayout[2].classification = VertexInputClassification::PerVertex;

#ifdef __vita__
		auto const source_vs = readFileBin("app0:assets/shaders/vita/texture2d_vs.gxp");
		vertexShaderDescription.sourceBinary = source_vs.data();
#else
		vertexShaderDescription.sourceCode = draw2dVSSource;
#endif
		vertexShaderDescription.verticesStride = sizeof(Vertex);

		auto result = device->createVertexShader(vertexShaderDescription);
		if (!result)
			return result.error();
		vertexShader = wob::move(result.value());

	}
	{
		BlendInfo blendInfo{};
		blendInfo.colorMask = ColorMaskBits::All;
		blendInfo.alphaOp = BlendOp::Add;
		blendInfo.colorOp = BlendOp::Add;
		blendInfo.alphaDst = BlendFactor::OneMinusDstAlpha;
		blendInfo.alphaSrc = BlendFactor::One;
		blendInfo.colorDst = BlendFactor::OneMinusSrcAlpha;
		blendInfo.colorSrc = BlendFactor::SrcAlpha;
		FragmentShaderDescription fragmentShaderDescription;
#ifdef __vita__
		auto const source_fs = readFileBin("app0:assets/shaders/vita/texture2d_fs.gxp");
		fragmentShaderDescription.sourceBinary = source_fs.data();
		fragmentShaderDescription.gxpVertexProgram = textureVertexShader.getGxpShader();
#else
		fragmentShaderDescription.sourceCode = draw2dFSSource;
		fragmentShaderDescription.blendInfo = &blendInfo;
#endif

		auto result = device->createFragmentShader(fragmentShaderDescription);
		if (!result)
			return result.error();
		fragmentShader = wob::move(result.value());
	}

	// init default sampler
	{
		SamplerDescription samplerDesc{};
		samplerDesc.addressU = TextureAddressMode::Clamp;
		samplerDesc.addressV = TextureAddressMode::Clamp;
		samplerDesc.filter = TextureFilter::Linear;
		samplerDesc.lodMin = 0;
		samplerDesc.lodBias = 0;
		auto result = device->createSampler(samplerDesc);
		if (!result)
			return result.error();
		sampler = wob::move(result.value());
	}

	// init default texture
	{
		TextureDescription textureDesc = {};
		textureDesc.cpuAccess = CPUAccessFlagBits::None;
		textureDesc.usage = MemoryUsage::Immutable;
		textureDesc.format = RHIFormat::R8G8B8A8_Uint;
		textureDesc.height = 1;
		textureDesc.width = 1;
		textureDesc.mipsLevel = 1;
		Color baseColor = Color::White;
		textureDesc.initialData = &baseColor;
		defaultTexture = device->createTexture(textureDesc).value();
	}

	ensureVertexBufferCapacity(200 * sizeof(Vertex));
	ensureIndexBufferCapacity(400 * sizeof(Index_t));

	// ensure at least one batch
	batches.emplace();

	WOB_LOG("draw2d initialized");
	return {};
}

void Draw2D::setColor(Color color)
{
	WOB_PROFILE_FUNCTION();
	currentState.color = color;
}

void Draw2D::setMatrix(mat3 const& mat)
{
	WOB_PROFILE_FUNCTION();
	currentState.transformationMatrix = mat;
}

void Draw2D::pushState()
{
	statesStack.push(currentState);
}

void Draw2D::popState()
{
	currentState = statesStack.back();
	statesStack.pop();
}

void Draw2D::addVertex(vec2 pos, vec2 uv)
{
	Vertex vtx;
	vtx.pos = currentState.transformationMatrix * pos;
	vtx.uv = uv;
	vtx.color = currentState.color.toVec4();
	vertices.push(vtx);
}

void Draw2D::addIndex(Index_t idx)
{
	indices.push(getCurrentBatch().offset + idx);
	getCurrentBatch().indexCount++;
}

void Draw2D::drawLine(Line2D const& line, float thickness)
{
	WOB_PROFILE_FUNCTION();

	vec2 const p1 = line.p1;
	vec2 const p2 = line.p2;

	vec2 dir = p2 - p1;
	float const len = dir.length();
	if (len < 1e-6f)
		return; // degenerate line

	dir = dir * (1.0f / len);

	float const halfSize = thickness * 0.5f;
	vec2 const normal = vec2(-dir.y, dir.x) * halfSize; // perpendicular

	vec2 const a = p1 + normal;
	vec2 const b = p1 - normal;
	vec2 const c = p2 - normal;
	vec2 const d = p2 + normal;

	drawQuad(a, d, c, b);
}

void Draw2D::drawPoint(vec2 p, float size)
{
	WOB_PROFILE_FUNCTION();
	drawLine({ {p.x - size, p.y}, {p.x + size, p.y} });
	drawLine({ {p.x, p.y - size}, {p.x, p.y + size} });
}

void Draw2D::drawQuad(vec2 const& a, vec2 const& b, vec2 const& c, vec2 const& d)
{
	WOB_PROFILE_FUNCTION();

	// Corners must be given in counterclockwise order (y-up).
	addVertex(a, vec2(0, 0));
	addVertex(b, vec2(0, 1));
	addVertex(c, vec2(1, 0));
	addVertex(d, vec2(1, 1));

	addIndex(0);
	addIndex(1);
	addIndex(2);
	addIndex(0);
	addIndex(2);
	addIndex(3);

	getCurrentBatch().offset += 6;
}

void Draw2D::drawRect(Rect const& rect)
{
	drawQuad(rect.p1, rect.p2, rect.p3, rect.p4);
}

void Draw2D::drawWireRect(Rect const& rect)
{

}

void Draw2D::drawTriangle(vec2 a, vec2 b, vec2 c)
{
	WOB_PROFILE_FUNCTION();

	// Signed area (2x). > 0 means counterclockwise in a y-up coordinate system.
	float const cross = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);

	if (cross == 0.0f)
		return; // degenerate triangle, nothing to draw


	addVertex(a, vec2(0, 0));
	addVertex(b, vec2(0, 1));
	addVertex(c, vec2(1, 1));

	// ensure indices are always clockwise
	if (cross < 0.0f)
	{
		addIndex(0);
		addIndex(1);
		addIndex(2);
	}
	else
	{
		addIndex(0);
		addIndex(2);
		addIndex(1);
	}
	getCurrentBatch().offset += 3;
}

void Draw2D::drawImage(RHITexture& texture, Rect const& rect)
{
	WOB_PROFILE_FUNCTION();
	batches.emplace();
	getCurrentBatch().texture = &texture;
	getCurrentBatch().firstIndex = indices.size();
	drawRect(rect);
}

// check https://github.com/ocornut/imgui/blob/master/imgui_draw.cpp#L3542
void Draw2D::drawText(FontRessource& font, StringView str, vec2 pos)
{
	WOB_PROFILE_FUNCTION();

	vec2 p = pos;
	for (uint i = 0; i < str.size(); i++)
	{
		auto const c = str[i];
		if (c == '\r') {
			p.x = pos.x;
			continue;
		}
		if (c == ' ') {
			p.x += 0.025f;
			continue;
		}
		if (c == '\t') {
			p.x += 0.1f;
			continue;
		}
		if (c == '\n') {
			p.x = pos.x;
			p.y -= font.yAdvance;
			continue;
		}

	//	currentState.texture = &font.texture;
	//	commands.push(Command{ DrawCommandType::Image, currentState });

		auto const glyph = font.getGlyph(c).value();
		vec2 const gsize = { glyph.u[1] - glyph.u[0], glyph.v[1] - glyph.v[0] };

		auto dp = p;
		dp.y = p.y - (gsize.y + glyph.yoff);

		vertices.push({ dp,								{ glyph.u[0], glyph.v[1]} });
		vertices.push({ vec2{dp.x + gsize.x, dp.y},	{ glyph.u[1], glyph.v[1]} });
		vertices.push({ vec2{dp.x, dp.y + gsize.y},	{ glyph.u[0], glyph.v[0]} });
		vertices.push({ (dp + gsize),					{ glyph.u[1], glyph.v[0]} });

		//vertices.push({ p,								{ glyph.u[0], glyph.v[1]} });
		//vertices.push({ vec2{p.x + gsize.x, p.y},	{ glyph.u[1], glyph.v[1]} });
		//vertices.push({ vec2{p.x, p.y + gsize.y},	{ glyph.u[0], glyph.v[0]} });
		//vertices.push({ (p + gsize),					{ glyph.u[1], glyph.v[0]} });

		p.x += gsize.x;
		//p.x += glyph.xadvance;

		indices.push(offset + 3);
		indices.push(offset + 1);
		indices.push(offset + 2);
		indices.push(offset + 0);
		offset += 4;
	}
}

void Draw2D::executeDrawCommands()
{
	WOB_PROFILE_FUNCTION();

	offset = 0;

	ensureVertexBufferCapacity(vertices.size() * sizeof(Vertex));
	ensureIndexBufferCapacity(indices.size() * sizeof(Index_t));
	device->setBufferData(vertexBuffer, vertices.data(), vertices.size() * sizeof(Vertex));
	device->setBufferData(indexBuffer, indices.data(), indices.size() * sizeof(uint16_t));

	uint colorIndicesOffset = 0;
	uint textureIndicesOffset = 0;

	device->bindFragmentSampler(sampler, 0);
	device->bindFragmentTexture(defaultTexture, 0);
	device->setVertexShader(vertexShader);
	device->setFragmentShader(fragmentShader);

	device->bindVertexBuffer(vertexBuffer, 0, sizeof(Vertex));
	device->bindIndexBuffer(indexBuffer, IndexTypeFormat::Uint16);
	device->setDrawPrimitiveMode(DrawPrimitiveType::TrianglesFill);

	for (auto const& batch : batches)
	{
		RHITexture& text = batch.texture != nullptr ? *batch.texture : defaultTexture;
		device->bindFragmentTexture(text, 0);
		device->drawIndexed(batch.indexCount, batch.firstIndex);
	}

	indices.clear();
	vertices.clear();
	batches.clear();
	batches.resize(1);
}


// @Review

Result<void> Draw2D::ensureVertexBufferCapacity(size_t sizeInBytes)
{
	WOB_PROFILE_FUNCTION();

	// reallocate buffers
	BufferDescription vertexBufferDesc{};
	vertexBufferDesc.bindFlags = BindFlagBits::VertexBuffer;
	vertexBufferDesc.usage = MemoryUsage::Dynamic;
	vertexBufferDesc.cpuAccessFlags = CPUAccessFlagBits::Write;
	vertexBufferDesc.sizeInBytes = sizeInBytes;
	
	return device->ensureBufferCapacity(vertexBuffer, vertexBufferDesc);
}

Result<void> Draw2D::ensureIndexBufferCapacity(size_t sizeInBytes)
{
	WOB_PROFILE_FUNCTION();

	// reallocate buffers
	BufferDescription indexBufferDesc{};
	indexBufferDesc.bindFlags = BindFlagBits::IndexBuffer;
	indexBufferDesc.usage = MemoryUsage::Dynamic;
	indexBufferDesc.cpuAccessFlags = CPUAccessFlagBits::Write;
	indexBufferDesc.sizeInBytes = sizeInBytes;

	return device->ensureBufferCapacity(indexBuffer, indexBufferDesc);
}

Draw2D::Batch& Draw2D::getCurrentBatch()
{
	return batches.back();
}


