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
	mainWindow.setKeyCallback({ [](InputAction action, int key, void* userData) {

		Engine& self = *(Engine*)userData;
		Key k = windowsToAESKey(key);
		if (action == InputAction::Pressed)
		{
			self.onKeyPressed(k);
			self.keyStates[k] = InputState::Down;
		}
		else
		{
			self.onKeyReleased(k);
			self.keyStates[k] = InputState::Up;
		}

		}, this });
	renderer.init(mainWindow);
	AES_LOG("engine initialized");
}

void Engine::run()
{
	using namespace std::chrono;
	AES_PROFILE_FUNCTION();

	double deltaTime = 0.0;

	start();
	while (!mainWindow.shouldClose())
	{
		AES_PROFILE_FRAME();
		
		auto start = std::chrono::high_resolution_clock::now();
		
		mainWindow.pollEvents();
		renderer.startFrame();

		update(deltaTime);

		renderer.endFrame();
		
		auto end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> deltaTimeInSec = end - start;
		deltaTime = deltaTimeInSec.count();
		time += deltaTime;
		frameCount++;
	}
}

InputState Engine::getKeyState(Key k) noexcept
{
	return keyStates[k];
}
