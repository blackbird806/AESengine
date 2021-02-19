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

		static Logger& getLogger() noexcept;
		
	protected:

		InputState getKeyState(Key k) noexcept;
		void getMousePos(float& x, float& y) const noexcept;

		Camera mainCamera;
		std::unique_ptr<Window> mainWindow;

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
		RHIRenderContext renderer;

		std::unordered_map<Key, InputState> keyStates;
	};
}

#endif // !ENGINE_HPP
