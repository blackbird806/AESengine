#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "core/aes.hpp"
#include "core/debug.hpp"
#include "core/window.hpp"
#include "renderer/D3D11renderer.hpp"
#include <unordered_map>

namespace aes {

	class Engine
	{
	public:

		struct InitInfo
		{
			const char* appName;
		};

		Engine(InitInfo const& info);
		~Engine();
	
		void init();
		void run();

	protected:

		InputState getKeyState(Key k) noexcept;

		Camera mainCamera;

	private:

		virtual void onKeyPressed(Key k) {};
		virtual void onKeyReleased(Key k) {};

		virtual void start() {}
		virtual void update(double deltaTime) {}
		virtual void draw() {}
		virtual void end() {}

		double time = 0.0;
		uint64_t frameCount = 0;
		
		const char* appName;
		Window mainWindow;
		D3D11Renderer renderer;

		std::unordered_map<Key, InputState> keyStates;
	};
}

#endif // !ENGINE_HPP
