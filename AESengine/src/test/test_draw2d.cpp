#include "tests.hpp"

#include "core/allocator.hpp"
#include "engine.hpp"
#include "renderer/draw2d.hpp"
#include "core/color.hpp"

using namespace aes;

class TestDraw2dApp : public Engine
{
public:

	aes::Draw2d draw2d;

	TestDraw2dApp(InitInfo const& info) : Engine(info)
	{
		AES_LOG("[TEST] DRAW2D");
	}

	void start() override
	{
		if (!draw2d.init())
			AES_FATAL_ERROR("draw2d creation failed");
	}

	void update(float deltaTime) override
	{
		draw2d.setColor(aes::Color::Red);
		draw2d.drawPoint({ 0, 0 });
		draw2d.drawPoint({ 0.4, 0 });
		draw2d.setColor(aes::Color::Blue);
		draw2d.drawPoint({ 0, 0.4 });
		draw2d.drawPoint({ 0.4, 0.4 });
		draw2d.setColor(aes::Color::White);
		draw2d.drawFillRect({ {0, 0}, {0.4, 0.4} });
	}

	void draw() override
	{
		draw2d.executeDrawCommands();
	}
};

void aes::test_draw2d()
{
	AES_START_PROFILE_SESSION("test draw2d startup");
	TestDraw2dApp app({
		.appName = "aes draw2d test"
		});
	app.init();
	auto startupSession = AES_STOP_PROFILE_SESSION();

	AES_START_PROFILE_SESSION("test draw2d running");
	app.run();
	auto runningSession = AES_STOP_PROFILE_SESSION();
}
