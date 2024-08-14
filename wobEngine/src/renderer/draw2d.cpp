#include "draw2d.hpp"

#include "core/utility.hpp"
#include "RHI/RHIRenderContext.hpp"
#include <glm/ext/matrix_clip_space.hpp>

#include "fontRenderer.hpp"

using namespace aes;

Result<void> Draw2d::init()
{
	AES_PROFILE_FUNCTION();

	// init shaders
	{
		VertexInputLayout vertexInputLayout[2];
		vertexInputLayout[0].semantic = SemanticType::Position;
		vertexInputLayout[0].parameterName = "aPosition";
		vertexInputLayout[0].offset = 0;
		vertexInputLayout[0].format = RHIFormat::R32G32_Float;

		vertexInputLayout[1].semantic = SemanticType::Color;
		vertexInputLayout[1].parameterName = "aColor";
		vertexInputLayout[1].offset = sizeof(vec4);
		vertexInputLayout[1].format = RHIFormat::R32G32B32A32_Float;

		vertexInputLayout[1].semantic = SemanticType::TexCoord;
		vertexInputLayout[1].parameterName = "aTexcoord";
		vertexInputLayout[1].offset = sizeof(vec2);
		vertexInputLayout[1].format = RHIFormat::R32G32_Float;

		VertexShaderDescription vertexShaderDescription;
#ifdef __vita__
		auto const source_vs = readFileBin("app0:assets/shaders/vita/texture2d_vs.gxp");
		vertexShaderDescription.source = source_vs.data();
#else
		vertexShaderDescription.source = readFile("assets/shaders/HLSL/texture2d.vs");
#endif
		vertexShaderDescription.verticesLayout = vertexInputLayout;
		vertexShaderDescription.verticesStride = sizeof(TextureVertex);

		AES_NOT_IMPLEMENTED();
		//auto err = textureVertexShader.init(vertexShaderDescription);
		//if (!err)
		//	return err;

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
		fragmentShaderDescription.source = source_fs.data();
		fragmentShaderDescription.gxpVertexProgram = textureVertexShader.getGxpShader();
#else
		fragmentShaderDescription.source = readFile("assets/shaders/HLSL/texture2d.fs");
		fragmentShaderDescription.blendInfo = blendInfo;
#endif
		//err = textureFragmentShader.init(fragmentShaderDescription);
		//if (!err)
		//	return err;
	}

	// init default sampler
	{
		SamplerDescription samplerDesc{};
		samplerDesc.addressU = TextureAddressMode::Repeat;
		samplerDesc.addressV = TextureAddressMode::Repeat;
		samplerDesc.filter = TextureFilter::Linear;
		samplerDesc.lodMin = 0;
		samplerDesc.lodBias = 0;
		//sampler.init(samplerDesc);
	}

	// init uniform buffer
	{
		BufferDescription bufferDesc{};
		bufferDesc.usage = MemoryUsage::Dynamic;
		bufferDesc.bindFlags = BindFlagBits::UniformBuffer;
		bufferDesc.cpuAccessFlags = CPUAccessFlagBits::Write;
		bufferDesc.sizeInBytes = sizeof(UniformBuffer);
		UniformBuffer defaultContent{};
		bufferDesc.initialData = &defaultContent;
		//auto err = uniformBuffer.init(bufferDesc);
		//if (!err)
		//	return err;
	}

	ensureTextureVertexBufferCapacity(200 * sizeof(TextureVertex));
	ensureTextureIndexBufferCapacity(400 * sizeof(Index_t));

	AES_LOG("draw2d initialized");
	return {};
}

void Draw2d::setColor(Color color)
{
	AES_PROFILE_FUNCTION();
	currentState.color = color;
}

void Draw2d::setMatrix(mat3 const& mat)
{
	AES_PROFILE_FUNCTION();
	currentState.transformationMatrix = mat;
}

void Draw2d::pushState()
{
	statesStack.push(currentState);
}

void Draw2d::popState()
{
	currentState = statesStack.back();
	statesStack.pop();
}

void Draw2d::drawLine(Line2D const& line)
{
	AES_PROFILE_FUNCTION();
	commands.push(Command{ DrawCommandType::Line, currentState.color });
	
	colorVertices.push({ line.p1, currentState.color });
	colorVertices.push({ line.p2, currentState.color });
	colorIndices.push(colorOffset + 0);
	colorIndices.push(colorOffset + 1);
	colorOffset += 2;
}

void Draw2d::drawPoint(vec2 p, float size)
{
	AES_PROFILE_FUNCTION();
	drawLine({ {p.x - size, p.y}, {p.x + size, p.y} });
	drawLine({ {p.x, p.y - size}, {p.x, p.y + size} });
}

void Draw2d::drawFillRect(Rect const& rect)
{
	AES_PROFILE_FUNCTION();
	commands.push(Command{ DrawCommandType::FillRect, currentState.color });

	RectBounds const bounds = rect.getBounds();
	colorVertices.push({ bounds.minL, currentState.color });
	colorVertices.push({ bounds.minR, currentState.color });
	colorVertices.push({ bounds.topL, currentState.color });
	colorVertices.push({ bounds.topR, currentState.color });

	colorIndices.push(colorOffset + 3);
	colorIndices.push(colorOffset + 1);
	colorIndices.push(colorOffset + 2);
	colorIndices.push(colorOffset + 0);
	colorOffset += 4;
}

void Draw2d::drawRect(Rect const& rect)
{
	AES_PROFILE_FUNCTION();
	RectBounds const b = rect.getBounds();
	drawLine({ b.minL, b.minR });
	drawLine({ b.minL, b.topL });
	drawLine({ b.topL, b.topR });
	drawLine({ b.minR, b.topR });
}

void Draw2d::drawImage(RHITexture& texture, Rect const& rect)
{
	AES_PROFILE_FUNCTION();

	commands.push(Command{ DrawCommandType::Image, currentState, &texture });

	RectBounds const bounds = rect.getBounds();
	textureVertices.push({ bounds.minL, {0, 1} });
	textureVertices.push({ bounds.minR, {1, 1} });
	textureVertices.push({ bounds.topL, {0, 0} });
	textureVertices.push({ bounds.topR, {1, 0} });

	textureIndices.push(textureOffset + 3);
	textureIndices.push(textureOffset + 1);
	textureIndices.push(textureOffset + 2);
	textureIndices.push(textureOffset + 0);
	textureOffset += 4;
}

// check https://github.com/ocornut/imgui/blob/master/imgui_draw.cpp#L3542
void Draw2d::drawText(FontRessource& font, std::string_view str, vec2 pos)
{
	AES_PROFILE_FUNCTION();

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

		commands.push(Command{ DrawCommandType::Image, currentState, &font.texture });
		auto const glyph = *font.getGlyph(c);
		vec2 const gsize = { glyph.u[1] - glyph.u[0], glyph.v[1] - glyph.v[0] };

		auto dp = p;
		dp.y = p.y - (gsize.y + glyph.yoff);

		textureVertices.push({ dp,								{ glyph.u[0], glyph.v[1]} });
		textureVertices.push({ vec2{dp.x + gsize.x, dp.y},	{ glyph.u[1], glyph.v[1]} });
		textureVertices.push({ vec2{dp.x, dp.y + gsize.y},	{ glyph.u[0], glyph.v[0]} });
		textureVertices.push({ (dp + gsize),					{ glyph.u[1], glyph.v[0]} });

		//textureVertices.push({ p,								{ glyph.u[0], glyph.v[1]} });
		//textureVertices.push({ vec2{p.x + gsize.x, p.y},	{ glyph.u[1], glyph.v[1]} });
		//textureVertices.push({ vec2{p.x, p.y + gsize.y},	{ glyph.u[0], glyph.v[0]} });
		//textureVertices.push({ (p + gsize),					{ glyph.u[1], glyph.v[0]} });

		p.x += gsize.x;
		//p.x += glyph.xadvance;

		textureIndices.push(textureOffset + 3);
		textureIndices.push(textureOffset + 1);
		textureIndices.push(textureOffset + 2);
		textureIndices.push(textureOffset + 0);
		textureOffset += 4;
	}
}

void Draw2d::executeDrawCommands()
{
	AES_PROFILE_FUNCTION();

	textureOffset = 0;

	//ensureTextureVertexBufferCapacity(textureVertices.size() * sizeof(TextureVertex));
	//ensureTextureIndexBufferCapacity(textureIndices.size() * sizeof(Index_t));
	//textureVertexBuffer.setData(textureVertices.data(), textureVertices.size() * sizeof(TextureVertex));
	//textureIndexBuffer.setData(textureIndices.data(), textureIndices.size() * sizeof(Index_t));

	auto& context = RHIRenderContext::instance();

	uint colorIndicesOffset = 0;
	uint textureIndicesOffset = 0;
	uint indicesCount;

	context.bindVSUniformBuffer(uniformBuffer, 0);
	
	context.setFragmentSampler(sampler, 0);
	context.setVertexShader(textureVertexShader);
	context.setFragmentShader(textureFragmentShader);

	context.bindVertexBuffer(textureVertexBuffer, sizeof(TextureVertex));
	context.bindIndexBuffer(textureIndexBuffer, IndexTypeFormat::Uint16);

	for (auto const& cmd : commands)
	{
		//uniformBuffer.setDataFromPOD(mat4(cmd.state.transformationMatrix));

		// @Review only draw triangles ?
		if (cmd.type == DrawCommandType::Line)
		{
			context.setDrawPrimitiveMode(DrawPrimitiveType::Lines);
			indicesCount = 2;
		}
		else
		{
			context.setDrawPrimitiveMode(DrawPrimitiveType::TriangleStrip);
			indicesCount = 4;
		}

		if (cmd.type == DrawCommandType::Image)
		{
			context.bindFragmentTexture(*cmd.texture, 0);
		}

		context.drawIndexed(indicesCount, textureIndicesOffset);
		textureIndicesOffset += indicesCount;
	}

	textureIndices.clear();
	textureVertices.clear();
	commands.clear();
}

// @Review

Result<void> Draw2d::ensureTextureVertexBufferCapacity(size_t sizeInBytes)
{
	AES_PROFILE_FUNCTION();

	// reallocate buffers
	BufferDescription vertexBufferDesc{};
	vertexBufferDesc.bindFlags = BindFlagBits::VertexBuffer;
	vertexBufferDesc.usage = MemoryUsage::Dynamic;
	vertexBufferDesc.cpuAccessFlags = CPUAccessFlagBits::Write;
	vertexBufferDesc.sizeInBytes = sizeInBytes;

	AES_NOT_IMPLEMENTED();
	//return ensureRHIBufferCapacity(textureVertexBuffer, vertexBufferDesc);
	return {};
}

Result<void> Draw2d::ensureTextureIndexBufferCapacity(size_t sizeInBytes)
{
	AES_PROFILE_FUNCTION();

	// reallocate buffers
	BufferDescription indexBufferDesc{};
	indexBufferDesc.bindFlags = BindFlagBits::IndexBuffer;
	indexBufferDesc.usage = MemoryUsage::Dynamic;
	indexBufferDesc.cpuAccessFlags = CPUAccessFlagBits::Write;
	indexBufferDesc.sizeInBytes = sizeInBytes;

	AES_NOT_IMPLEMENTED();
	//return ensureRHIBufferCapacity(textureIndexBuffer, indexBufferDesc);
	return {};
}


