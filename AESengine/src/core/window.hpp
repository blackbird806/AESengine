#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "aes.hpp"
#include "os.hpp"

namespace aes {

	enum class InputAction {
		Pressed,
		Released
	};

	class Window
	{

	public:

		using ResizeCallbackT = void(*)(uint, uint);
		using KeyCallbackT = void(*)(InputAction action, int key);
		using MouseMoveCallbackT = void(*)(int mouseX, int mouseY);

		static LRESULT CALLBACK windowProcess(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		
		explicit Window(const char* name);

		void setResizeCallback(ResizeCallbackT func);
		void setKeyCallback(KeyCallbackT func);
		void setMouseMoveCallback(MouseMoveCallbackT func);
		void getMousePosition(int& mouseX, int& mouseY) const;
		void getScreenSize(uint& sizeX, uint& sizeY) const;

		HWND getHandle() const;

		void open();
		void setVisible(bool);
		void close();

		void pollEvents();
		bool shouldClose() const;

	private:

		bool shouldClose_ = false;
		HWND handle;
		KeyCallbackT keyCallback = nullptr;
		ResizeCallbackT resizeCallback = nullptr;
		MouseMoveCallbackT mouseMoveCallback = nullptr;
		uint width, height;
		int mouseX, mouseY;
	};

}
#endif // !WINDOW_HPP
