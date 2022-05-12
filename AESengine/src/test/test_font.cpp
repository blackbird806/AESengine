#include "tests.hpp"

#include "core/allocator.hpp"
#include "core/array.hpp"
#include "engine.hpp"
#include "renderer/fontRenderer.hpp"
#include "renderer/draw2d.hpp"
#include "core/utility.hpp"
#include "core/geometry.hpp"
#include "core/color.hpp"

using namespace aes;

class TestFontApp : public Engine
{
public:

	aes::FontRessource defaultFont;
	aes::Draw2d draw2d;

	TestFontApp(InitInfo const& info) : Engine(info), defaultFont(aes::globalAllocator), draw2d(aes::globalAllocator)
	{
		AES_LOG("[TEST] FONTS");
	}

	void start() override
	{
		auto fontData = readFileBin("assets/fonts/courier.ttf");
		defaultFont = createFontRessource(aes::globalAllocator, fontData).value();

		if (!draw2d.init())
			AES_FATAL_ERROR("draw2d creation failed");

	}

	void update(float deltaTime) override
	{
		draw2d.drawText(defaultFont, "hello world", { 0, 0 });
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
