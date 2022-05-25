#include <iostream>
#include <glm/gtx/matrix_transform_2d.hpp>

#include "tests.hpp"

#include "core/allocator.hpp"
#include "engine.hpp"
#include "renderer/fontRenderer.hpp"
#include "renderer/draw2d.hpp"
#include "core/utility.hpp"
#include "core/color.hpp"
#include "renderer/textureUtility.hpp"

using namespace aes;

class TestFontApp : public Engine
{
public:

	RHITexture checkboard;
	FontRessource defaultFont;
	Draw2d draw2d;

	TestFontApp(InitInfo const& info) : Engine(info), defaultFont(aes::globalAllocator), draw2d(aes::globalAllocator)
	{
		AES_LOG("[TEST] FONTS");
	}

	void start() override
	{
		{
			uint const w = 1024, h = 1024;
			Array<Color> colors(globalAllocator);
			colors.resize(w * h);
			buildCheckboard(colors, w, h, Color::Green, Color::Blue, 0.25f);
			TextureDescription desc;
			desc.width = w;
			desc.height = h;
			desc.format = RHIFormat::R8G8B8A8_Uint;
			desc.cpuAccess = CPUAccessFlagBits::None;
			desc.usage = MemoryUsage::Immutable;
			desc.initialData = colors.data();
			desc.mipsLevel = 1;
			checkboard.init(desc);
		}

		{ 
			auto const fontData = readFileBin("assets/fonts/ProggyClean.ttf");
			FontParams params{};
			params.fontData = fontData;
			params.fontSize = 60;
			params.oversampling = 1;
			auto fontResult = createFontRessource(aes::globalAllocator, params);

			if (!fontResult)
				AES_FATAL_ERROR("font creation failed");

			defaultFont = std::move(fontResult.value());
		}

		if (!draw2d.init())
			AES_FATAL_ERROR("draw2d creation failed");
	}

	void update(float deltaTime) override
	{
		uint wx, wy;
		mainWindow->getScreenSize(wx, wy);
		float const aspect = (float)wx / wy;

		float xstart = 0.0;
		static float scale = 1.0;

		if (isKeyDown(Key::Down))
			scale -= 0.1 * deltaTime;
		if (isKeyDown(Key::Up))
			scale += 0.1 * deltaTime;
		if (isKeyPressed(Key::Space))
			scale = 1.0;

		draw2d.setMatrix(glm::scale(glm::mat3(1.0f), { scale, scale * aspect }));
		draw2d.drawText(defaultFont, fmt::format("hello world\nscale {}", scale), {xstart, 0.0});

		draw2d.drawImage(defaultFont.texture, { {-0.5, -0.5}, {0.5, 0.5}});
		//draw2d.drawImage(checkboard, { {-1, -1}, {0.0, 0.0}});
	}

	void draw() override
	{
		static ID3D11BlendState* pBlendState;;
		AES_ONCE(
			D3D11_BLEND_DESC desc;
			ZeroMemory(&desc, sizeof(desc));
			desc.AlphaToCoverageEnable = false;
			desc.RenderTarget[0].BlendEnable = true;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
			RHIRenderContext::instance().getDevice()->CreateBlendState(&desc, &pBlendState));
		const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
		RHIRenderContext::instance().getDeviceContext()->OMSetBlendState(pBlendState, blend_factor, 0xffffffff);

		draw2d.executeDrawCommands();
	}
};

void aes::test_fonts()
{
	AES_START_PROFILE_SESSION("test font startup");
	TestFontApp app({
		.appName = "aes font test"
	});
	app.init();
	auto startupSession = AES_STOP_PROFILE_SESSION();

	AES_START_PROFILE_SESSION("test font running");
	app.run();
	auto runningSession = AES_STOP_PROFILE_SESSION();
}
