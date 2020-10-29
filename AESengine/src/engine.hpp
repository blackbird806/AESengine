#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "core/window.hpp"
#include "renderer/D3D11renderer.hpp"

namespace aes {

	class Engine
	{
	public:

		struct InitInfo
		{
			const char* appName;
		};

		Engine(InitInfo const& info);
		void init();
		void run();

	private:

		const char* appName;
		Window mainWindow;
		D3D11Renderer renderer;
	};
}

#endif // !ENGINE_HPP
