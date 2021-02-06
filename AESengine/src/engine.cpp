#include "engine.hpp"
#include "core/debug.hpp"

#ifdef _WIN32
	#include "core/platformWindows/win_window.hpp"
#endif

#include <chrono>

using namespace aes;

Engine::Engine(InitInfo const& info) :
#ifdef _WIN32
	mainWindow(std::make_unique<Win_Window>(info.appName)),
#else
	mainWindow(std::make_unique<EmptyWindow>())
#endif
	appName(info.appName)
{

}

Engine::~Engine()
{
	renderer.destroy();
	mainWindow->close();
}

void Engine::init()
{
	AES_PROFILE_FUNCTION();
	mainWindow->open();
	mainWindow->setKeyCallback({ [](InputAction action, int key, void* userData) {

		Engine& self = *static_cast<Engine*>(userData);
		Key const k = windowsToAESKey(key);
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

	renderer.init(*mainWindow);
	AES_LOG("engine initialized");
}

void Engine::run()
{
	using namespace std::chrono;
	AES_PROFILE_FUNCTION();

	double deltaTime = 0.0;

	start();
	while (!mainWindow->shouldClose())
	{
		AES_PROFILE_FRAME();
		auto start = std::chrono::high_resolution_clock::now();
		
		mainWindow->pollEvents();
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

// return normalized mouse position
// 0 0 is up left
// 1 1 is down right
void Engine::getMousePos(float& x, float& y) const noexcept
{
	uint sx, sy;
	mainWindow->getScreenSize(sx, sy);
	int mx, my;
	mainWindow->getMousePosition(mx, my);
	x = (float)mx / (float)sx;
	y = (float)my / (float)sy;
}
