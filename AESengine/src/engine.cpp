#include "engine.hpp"
#include "core/debug.hpp"

#include <chrono>
#include <glm/gtc/matrix_transform.hpp>

using namespace aes;

Engine::Engine(InitInfo const& info) : appName(info.appName), mainWindow(appName)
{

}

Engine::~Engine()
{
	renderer.destroy();
	mainWindow.close();
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
	mainCamera.projMatrix = glm::perspectiveLH_ZO(glm::radians(45.0f), 16.0f / 9.0f, 0.0001f, 1000.0f);
	while (!mainWindow.shouldClose())
	{
		AES_PROFILE_FRAME();
		auto start = std::chrono::high_resolution_clock::now();
		
		mainWindow.pollEvents();
		renderer.startFrame(mainCamera);

		update(deltaTime);
		draw();

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
