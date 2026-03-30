#include "engine.hpp"
#include "core/debug.hpp"
#include "core/uniquePtr.hpp"
#include "core/time.hpp"
#include "core/ranges.hpp"

#ifdef _WIN32
	#include "core/platformWindows/win_window.hpp"
#else
	#include "core/Window.hpp"
#endif

using namespace wob;

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
	mainWindow->close();
	WOB_LOG("engine destroyed");
}

void Engine::init()
{
	WOB_PROFILE_FUNCTION();
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

	WOB_LOG("engine initialized");
}

void Engine::run()
{
	WOB_PROFILE_FUNCTION();

	float deltaTime = 0.0;

	start();
	while (!mainWindow->shouldClose())
	{
		WOB_PROFILE_FRAME();
		int64_t start = getPerfCount();

		mainWindow->pollEvents();

		update(deltaTime);
		draw();

		keyJustPressed.clear();

		int64_t const end = getPerfCount();
		double const deltaTimeInSec = (static_cast<double>(end) - start) / getCPUFrequency();
		deltaTime = deltaTimeInSec;
		time += deltaTime;
		frameCount++;
	}
}

const char* Engine::getEngineShaderPath() const
{
	return WOB_DEFAULT_ENGINE_SHADER_PATH;
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
	return wob::ranges::find(keyJustPressed, k) != keyJustPressed.end();
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

