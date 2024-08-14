#include "core/allocator.hpp"
#include "engine.hpp"
#include "core/utility.hpp"
#include "core/color.hpp"
#include "renderer/textureUtility.hpp"
#include "renderer/fontRenderer.hpp"

using namespace aes;

const char* fontPath = "../../../../wobEngine/assets/fonts/";

class TestFontApp : public Engine
{
public:

	RHITexture checkboard;

	TestFontApp(InitInfo const& info) : Engine(info)
	{
		AES_LOG("[TEST] FONTS");
	}

	void start() override
	{
		{ 
			auto const fontData = readFileBin("../../../../wobEngine/assets/fonts/courier.ttf");
			FontParams params{};
			params.fontData = fontData;
			params.fontSize = 25;
			params.oversampling = 2;
			auto fontResult = createFontRessource(params);

			//if (!fontResult)
			//	AES_FATAL_ERROR("font creation failed");

			//defaultFont = std::move(fontResult.value());
		}

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


	}

	void draw() override
	{
	}
};

int main()
{
	AES_START_PROFILE_SESSION("test font startup");
	TestFontApp app({
		.appName = "aes font test"
	});
	app.init();
	auto startupSession = AES_STOP_PROFILE_SESSION();

	AES_START_PROFILE_SESSION("test font running");
	//app.run();
	auto runningSession = AES_STOP_PROFILE_SESSION();
	return 0;
}
