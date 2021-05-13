#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "core/aes.hpp"
#include "core/debug.hpp"
#include "core/window.hpp"
#include "renderer/RHI/RHIRenderContext.hpp"
#include "renderer/camera.hpp"
#include <unordered_map>
#include <memory>

namespace aes {

	class Engine
	{
	public:

		struct InitInfo
		{
			const char* appName;
		};

		Engine(InitInfo const& info);
		virtual ~Engine();
	
		void init();
		void run();

	protected:

		InputState getKeyState(Key k) noexcept;

		/*
		 *0;1
		 * ---------- 1;1
		 * |		|
		 * |		|
		 * ----------
		 * 0;0		1;0
		 */
		void getViewportMousePos(float& x, float& y) const noexcept;

		/*
		 * 0;h
		 * ---------- w;h
		 * |		|
		 * |		|
		 * ----------
		 * 0;0		w;0
		 */
		void getScreenMousePos(int& x, int& y) const noexcept;

		Camera mainCamera;
		std::unique_ptr<Window> mainWindow;

	private:

		virtual void onKeyPressed(Key k) {};
		virtual void onKeyReleased(Key k) {};

		virtual void start() {}
		virtual void update(float deltaTime) {}
		virtual void draw() {}

		double time = 0.0;
		uint64_t frameCount = 0;

		const char* appName;
		RHIRenderContext renderer;

		std::unordered_map<Key, InputState> keyStates;
	};
}

#endif // !ENGINE_HPP
