#include "window.hpp"
#include "aes.hpp"

#include <string>

using namespace aes;

void Window::setResizeCallback(ResizeCallbackT func)
{
	resizeCallback = func;
}

void Window::setKeyCallback(KeyCallbackT func)
{
	keyCallback = func;
}

void Window::setMouseMoveCallback(MouseMoveCallbackT func)
{
	mouseMoveCallback = func;
}

void Window::setMouseWheelMoveCallback(MouseWheelMoveCallbackT func)
{
	mouseWheelMoveCallback = func;
}

void Window::getMousePosition(int& mouseX_, int& mouseY_) const
{
	AES_PROFILE_FUNCTION();
	mouseX_ = mouseX;
	mouseY_ = mouseY;
}

void Window::getScreenSize(uint& sizeX, uint& sizeY) const
{
	AES_PROFILE_FUNCTION();
	sizeX = width;
	sizeY = height;
}

bool Window::shouldClose() const
{
	return shouldClose_;
}

