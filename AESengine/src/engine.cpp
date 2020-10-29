#include "engine.hpp"
#include "core/debug.hpp"

#include <chrono>

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
	using namespace std::chrono;
	AES_PROFILE_FUNCTION();

	double deltaTime = 0.0;
	double time = 0.0;
	uint64_t frameCount = 0;
	while (!mainWindow.shouldClose())
	{
		AES_PROFILE_FRAME();
		
		auto start = std::chrono::high_resolution_clock::now();
		
		mainWindow.pollEvents();
		renderer.startFrame();



		renderer.endFrame();
		
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> deltaTimeInSec = end - start;
		deltaTime = deltaTimeInSec.count();
		time += deltaTime;
		frameCount++;
	}
}
