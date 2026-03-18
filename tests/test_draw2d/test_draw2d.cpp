#include "core/allocator.hpp"
#include "engine.hpp"
#include "renderer/draw2d.hpp"
#include "core/color.hpp"
#include "core/vec2.hpp"
#include "renderer/RHI/RHIDevice.hpp"

using namespace wob;

class TestDraw2dApp : public Engine
{
public:

	RHIDevice device;
	wob::Draw2D draw2d;

	TestDraw2dApp(InitInfo const& info) : Engine(info)
	{
		WOB_LOG("[TEST] DRAW2D");
	}

	void start() override
	{
		initializeGraphicsAPI();
		if (!draw2d.init(device))
			WOB_FATAL_ERROR("draw2d creation failed");
	}

	void update(float deltaTime) override
	{
		draw2d.setColor(wob::Color::Red);
		draw2d.drawPoint({ 0, 0 });
		draw2d.drawPoint({ 0.4, 0 });
		draw2d.setColor(wob::Color::Blue);
		draw2d.drawPoint({ 0, 0.4 });
		draw2d.drawPoint({ 0.4, 0.4 });
		draw2d.setColor(wob::Color::White);
		draw2d.drawFillRect({ {0, 0}, {0.4, 0.4} });
	}

	void draw() override
	{
		draw2d.executeDrawCommands();
	}
};

int main()
{
	//WOB_START_PROFILE_SESSION("test draw2d startup");
	TestDraw2dApp app({
		.appName = "aes draw2d test"
		});
	app.init();
	//auto startupSession = WOB_STOP_PROFILE_SESSION();

	//WOB_START_PROFILE_SESSION("test draw2d running");
	app.run();
	//auto runningSession = WOB_STOP_PROFILE_SESSION();
	return 0;
}
