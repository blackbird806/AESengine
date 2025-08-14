#ifndef AES_ENGINE_HPP
#define AES_ENGINE_HPP

#include "core/aes.hpp"
#include "core/debug.hpp"
#include "core/window.hpp"
#include "core/array.hpp"
#include "core/uniquePtr.hpp"
#include "renderer/RHI/RHIRenderContext.hpp"

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

		const char* getEngineShaderPath() const;

	protected:

		InputState getKeyState(Key k) noexcept;
		bool isKeyDown(Key k) noexcept;
		// return true only the first frame
		bool isKeyPressed(Key k) noexcept;

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

		UniquePtr<Window> mainWindow;

	private:

		virtual void onKeyPressed(Key k) {};
		virtual void onKeyReleased(Key k) {};

		virtual void start() {}
		virtual void update(float deltaTime) {}
		virtual void draw() {}

		double time = 0.0;
		uint64_t frameCount = 0;

		const char* appName;

		Array<InputState> keyStates;
		Array<Key> keyJustPressed;
	};
}

#endif // !ENGINE_HPP
