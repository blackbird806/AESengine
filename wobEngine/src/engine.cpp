#include "engine.hpp"
#include "core/debug.hpp"

#ifdef _WIN32
	#include "core/platformWindows/win_window.hpp"
#else
	#include "core/Window.hpp"
#endif

#include <chrono>

using namespace aes;

Engine::Engine(InitInfo const& info) :
#ifdef _WIN32
	mainWindow(makeUnique<Win_Window>(info.appName)),
#else
	mainWindow(makeUnique<EmptyWindow>()),
#endif
	appName(info.appName)
{

}

Engine::~Engine()
{
	renderer.destroy();
	mainWindow->close();
	AES_LOG("engine destroyed");
}

void Engine::init()
{
	AES_PROFILE_FUNCTION();
	mainWindow->open();
	keyStates.resize((int)Key::Max);
	mainWindow->setKeyCallback({ [](InputAction action, int key, void* userData) {

		Engine& self = *static_cast<Engine*>(userData);
		Key const k = windowsToAESKey(key);
		
		int const keyIndex = (int)k;
		if (action == InputAction::Pressed)
		{
			self.onKeyPressed(k);
			if (self.keyStates[keyIndex] == InputState::Up)
				self.keyJustPressed.push(k);

			self.keyStates[keyIndex] = InputState::Down;
		}
		else
		{
			self.onKeyReleased(k);
			self.keyStates[keyIndex] = InputState::Up;
		}

		}, this });

	renderer.init(*mainWindow);
	AES_LOG("engine initialized");
}

void Engine::run()
{
	using namespace std::chrono;
	AES_PROFILE_FUNCTION();

	float deltaTime = 0.0;

	start();
	while (!mainWindow->shouldClose())
	{
		AES_PROFILE_FRAME();
		auto const start = std::chrono::high_resolution_clock::now();

		mainWindow->pollEvents();
		renderer.startFrame();

		update(deltaTime);
		draw();

		renderer.endFrame();
		keyJustPressed.clear();

		auto const end = std::chrono::high_resolution_clock::now();
		std::chrono::duration<float> const deltaTimeInSec = end - start;
		deltaTime = deltaTimeInSec.count();
		time += deltaTime;
		frameCount++;
	}
}

InputState Engine::getKeyState(Key k) noexcept
{
	return keyStates[(int)k];
}

bool Engine::isKeyDown(Key k) noexcept
{
	return keyStates[(int)k] == InputState::Down;
}

bool Engine::isKeyPressed(Key k) noexcept
{
	return std::ranges::find(keyJustPressed, k) != keyJustPressed.end();
}

void Engine::getViewportMousePos(float& x, float& y) const noexcept
{
	uint sx, sy;
	mainWindow->getScreenSize(sx, sy);
	int mx, my;
	mainWindow->getMousePosition(mx, my);
	x = (float)mx / (float)sx;
	y = (float)my / (float)sy;
}

void Engine::getScreenMousePos(int& x, int& y) const noexcept
{
	mainWindow->getMousePosition(x, y);
}

