#include <iostream>
#include <glm/gtx/matrix_transform_2d.hpp>

#include "tests.hpp"

#include "core/allocator.hpp"
#include "engine.hpp"
#include "renderer/fontRenderer.hpp"
#include "renderer/draw2d.hpp"
#include "core/utility.hpp"
#include "core/color.hpp"

using namespace aes;

static Array<uint32_t> createCheckboard(uint resX, uint resY, float segmentSizef = 0.1f)
{
	uint const segmentSizeX = std::round(resX * segmentSizef);
	uint const segmentSizeY = std::round(resY * segmentSizef);

	Array<uint32_t> bitmap(aes::globalAllocator);
	bitmap.resize(resX * resY);
	for (uint i = 0; i < resX; i++)
	{
		uint const segmentNumI = i / segmentSizeX;
		for (uint j = 0; j < resY; j++)
		{
			int const segmentNumJ = j / segmentSizeY;

			bitmap[i + j * resX] = segmentNumI % 2 != segmentNumJ % 2 ? Color::Blue : Color::Red;
		}
	}
	return bitmap;
}

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
			auto colors = createCheckboard(w, h);
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

		auto fontData = readFileBin("assets/fonts/courier.ttf");
		auto fontResult = createFontRessource(aes::globalAllocator, fontData);

		if (!fontResult)
			AES_FATAL_ERROR("font creation failed");

		defaultFont = std::move(fontResult.value());
		//defaultFont.texture = std::move(checkboard);

		if (!draw2d.init())
			AES_FATAL_ERROR("draw2d creation failed");
	}

	void update(float deltaTime) override
	{
		uint wx, wy;
		mainWindow->getScreenSize(wx, wy);
		float aspect = (float)wx / wy;

		float xstart = -0.25;

		/*draw2d.setMatrix(glm::scale(glm::mat3(1.0f), { 0.25, 0.25 * aspect }));
		draw2d.drawText(defaultFont, "hello world", { xstart, 1.0 });*/

		draw2d.setMatrix(glm::scale(glm::mat3(1.0f), { 0.5, 0.5 * aspect }));
		draw2d.drawText(defaultFont, "hello world", { xstart, 0.5 });
		draw2d.drawPoint({ xstart, 0.5});

		draw2d.setMatrix(glm::scale(glm::mat3(1.0f), { 1.0, aspect }));
		draw2d.drawText(defaultFont, "hello world", { xstart, 0.0 });
		draw2d.drawPoint({ xstart, 0 });

		draw2d.setMatrix(glm::scale(glm::mat3(1.0f), { 2.0, 2.0 * aspect }));
		draw2d.drawText(defaultFont, "hello world", { xstart, -0.15 });

		draw2d.setMatrix(glm::mat3(1.0f));

		draw2d.drawImage(defaultFont.texture, { {-1, -1}, {1, 1}});
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
