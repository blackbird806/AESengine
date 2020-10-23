#include "engine.hpp"
#include "core/debug.hpp"

using namespace aes;

Engine::Engine(InitInfo const& info) : appName(info.appName), mainWindow(appName)
{

}

void Engine::init()
{
	AES_PROFILE_FUNCTION();
	mainWindow.open();
	renderer.init(mainWindow);
	AES_LOG("engine initialized");
}

void Engine::run()
{
	AES_PROFILE_FUNCTION();
	while (!mainWindow.shouldClose())
	{
		AES_PROFILE_FRAME();
		mainWindow.pollEvents();

		renderer.startFrame();
		renderer.endFrame();
	}
}
