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
			auto const fontData = readFileBin("assets/fonts/courier.ttf");
			FontParams params{};
			params.fontData = fontData;
			params.fontSize = 15;
			params.oversampling = 2;
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
		float aspect = (float)wx / wy;

		float xstart = -0.25;

		draw2d.setMatrix(glm::scale(glm::mat3(1.0f), { 1.0, aspect }));
		draw2d.drawText(defaultFont, "hello world", { xstart, 0.0 });
		//draw2d.drawImage(defaultFont.texture, { {-1, -1}, {1, 1}});
		draw2d.drawImage(checkboard, { {-1, -1}, {0.0, 0.0}});
	}

	void draw() override
	{
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
