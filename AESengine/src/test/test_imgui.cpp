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
#include "UI/imgui.hpp"

using namespace aes;

class TestImguiApp : public Engine
{
public:
	RHIDevice device;
	RHITexture checkboard;
	FontRessource defaultFont;
	Draw2d draw2d;
	ImguiContext imgui;

	TestImguiApp(InitInfo const& info) : Engine(info)
	{
		AES_LOG("[TEST] IMGUI");
	}

	void start() override
	{
		{
			auto const fontData = readFileBin("assets/fonts/ProggyClean.ttf");
			FontParams params{};
			params.device = &device;
			params.fontData = fontData;
			params.fontSize = 32;
			params.oversampling = 2;
			auto fontResult = createFontRessource(params);

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

		int mx, my;
		mainWindow->getMousePosition(mx, my);

		InputData indata;
		indata.cursorPressed = isKeyPressed(Key::LClick);
		indata.cursorDown = isKeyDown(Key::LClick);
		indata.cursorPos = glm::vec2(((float)mx / wx - 0.5) * 2, ((float)my / wy - 0.5) * 2);
		imgui.updateInputData(indata);

		imgui.beginFrame();

		imgui.beginWindow("debug");
		imgui.label(fmt::format("deltaTime : {:.3}", deltaTime));
		imgui.label(fmt::format("fps : {}", 60 / deltaTime));
		imgui.label(fmt::format("mouse pos {:.2} {:.2}", indata.cursorPos.x, indata.cursorPos.y));
		imgui.label(fmt::format("mouse state {}", indata.cursorDown));
		imgui.label("[TEST] \\/*.,-_=+^");
		imgui.label("hello world");
		if (imgui.button("button 1"))
		{
			AES_LOG("Button 1 pressed");
		}
		static float f = 0.0f;
		if (imgui.sliderFloat("slider", f))
		{
			AES_LOG("Slider val {}", f);
		}
		imgui.label(fmt::format("f {}", f));
		imgui.endWindow();
	}

	void draw() override
	{
		for (auto const& cmd : imgui.drawCmds)
		{
			switch (cmd.type)
			{
			case ImguiDrawCmdType::Text:
			{
				auto arg = std::get<TextArg>(cmd.arg);
				draw2d.drawText(defaultFont, arg.str, arg.pos);
			}
				break;
			case ImguiDrawCmdType::Rect: break;
			case ImguiDrawCmdType::FillRect:
			{
				auto arg = std::get<RectArg>(cmd.arg);
				draw2d.setColor(arg.col);
				draw2d.drawFillRect(arg.rect);
			}
				break;
			case ImguiDrawCmdType::Image: 
				break;
			}
		}

		draw2d.executeDrawCommands();
	}
};

void aes::test_imgui()
{
	AES_START_PROFILE_SESSION("test imgui startup");
	TestImguiApp app({
		.appName = "aes imgui test"
		});
	app.init();
	auto startupSession = AES_STOP_PROFILE_SESSION();

	AES_START_PROFILE_SESSION("test imgui running");
	app.run();
	auto runningSession = AES_STOP_PROFILE_SESSION();
}
