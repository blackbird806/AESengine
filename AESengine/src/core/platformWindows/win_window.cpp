#include "win_window.hpp"
#include "core/string_converter.hpp"

#include <string>
#include <windowsx.h>
#define NEAR 
#include <dwmapi.h>

#include "core/maths.hpp"

#define AES_CLASSNAME L"AES_CLASS"
#define WINDOW_HANDLE_PROP_NAME L"AES_WinHandle"

using namespace aes;

LRESULT Win_Window::windowProcess(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Win_Window* self = (Win_Window*)GetPropW(hwnd, WINDOW_HANDLE_PROP_NAME);
	switch (msg)
	{
		case WM_CREATE:
		{
			//RECT size_rect;
			//GetWindowRect(hwnd, &size_rect);

			//// Inform the application of the frame change to force redrawing with the new
			//// client area that is extended into the title bar
			//SetWindowPos(
			//	hwnd, NULL,
			//	size_rect.left, size_rect.top,
			//	size_rect.right - size_rect.left, size_rect.bottom - size_rect.top,
			//	SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE
			//);
			break;
		}
		case WM_ACTIVATE:
		{
			break;
		}
		case WM_NCCALCSIZE: {
			//UINT dpi = GetDpiForWindow(hwnd);

			//int frame_x = GetSystemMetricsForDpi(SM_CXFRAME, dpi);
			//int frame_y = GetSystemMetricsForDpi(SM_CYFRAME, dpi);
			//int padding = GetSystemMetricsForDpi(SM_CXPADDEDBORDER, dpi);

			//NCCALCSIZE_PARAMS* params = (NCCALCSIZE_PARAMS*)lParam;
			//RECT* requested_client_rect = params->rgrc;

			//requested_client_rect->right -= frame_x + padding;
			//requested_client_rect->left += frame_x + padding;
			//requested_client_rect->bottom -= frame_y + padding;
			break;
		}
		// workaround TODO
		case WM_LBUTTONDOWN:
		{
			if (self->keyCallback) {
				self->keyCallback(InputAction::Pressed, VK_LBUTTON);
			}
			break;
		}
		case WM_RBUTTONDOWN:
		{
			if (self->keyCallback) {
				self->keyCallback(InputAction::Pressed, VK_RBUTTON);
			}
			break;
		}
		case WM_MBUTTONDOWN:
		{
			if (self->keyCallback) {
				self->keyCallback(InputAction::Pressed, VK_MBUTTON);
			}
			break;
		}
		case WM_LBUTTONUP:
		{
			if (self->keyCallback) {
				self->keyCallback(InputAction::Released, VK_LBUTTON);
			}
			break;
		}
		case WM_MBUTTONUP:
		{
			if (self->keyCallback) {
				self->keyCallback(InputAction::Released, VK_MBUTTON);
			}
			break;
		}
		case WM_RBUTTONUP:
		{
			if (self->keyCallback) {
				self->keyCallback(InputAction::Released, VK_RBUTTON);
			}
			break;
		}
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			if (self->keyCallback) {
				InputAction const action = (HIWORD(lParam) & KF_UP) ? InputAction::Released : InputAction::Pressed;
				self->keyCallback(action, wParam);
			}
			break;
		}
		case WM_MOUSEMOVE:
			self->mouseX = GET_X_LPARAM(lParam);
			self->mouseY = self->height - GET_Y_LPARAM(lParam); // 0 is down
			if (self->mouseMoveCallback) self->mouseMoveCallback(self->mouseX, self->mouseY);
			break;
		// For some reason WM_MOUSEWHEEL is defined 0x020E on my win api and doesn't works
		// With 0x020A as documented in https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-mousewheel everything works properly
		case 0x020A: // WM_MOUSEWHEEL
			{
				if (self->mouseWheelMoveCallback) self->mouseWheelMoveCallback(aes::sign(GET_WHEEL_DELTA_WPARAM(wParam)));
				break;
			}
		case WM_SIZE:
		//	self->width = LOWORD(lParam);
		//	self->height = HIWORD(lParam);
			if (!self)
				break;
			RECT rect;
			if (GetClientRect(hwnd, &rect))
			{
				self->width = rect.right - rect.left;
				self->height = rect.bottom - rect.top;
			}
			else
			{
				self->width = 0;
				self->height = 0;
			}
			// do not call callback onMinimize
			if (self->resizeCallback && self->width > 0 && self->height > 0) 
				self->resizeCallback(self->width, self->height);
			break;
		case WM_DESTROY: {
			self->shouldClose_ = true;
			return 0;
		}
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

Win_Window::Win_Window(const char* name)
{
	AES_PROFILE_FUNCTION();
	
	AES_ASSERT(name);

	std::wstring const windowName = to_wstring(std::string(name));

	WNDCLASSEX wc = { };
	
	HINSTANCE const hInstance = GetModuleHandle(NULL);
	WNDPROC const winPRoc = (WNDPROC)windowProcess;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = winPRoc;
	wc.hInstance = hInstance;
	wc.lpszClassName = AES_CLASSNAME;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

	RegisterClassEx(&wc);
	
	handle = CreateWindowEx(
		0,                          // Optional window styles.
		AES_CLASSNAME,				// Window class
		windowName.c_str(),			// Window text
		WS_OVERLAPPEDWINDOW,        // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	SetPropW(handle, WINDOW_HANDLE_PROP_NAME, this);
	AES_ASSERT(handle != NULL);
	
	RECT rect;
	if (GetClientRect(handle, &rect))
	{
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}
	else
	{
		width = 0;
		height = 0;
	}
}

Win_Window::~Win_Window()
{
	close();
}

void* Win_Window::getHandle() const
{
    return handle;
}

void Win_Window::open()
{
	ShowWindow(handle, SW_SHOWNORMAL);
}

void Win_Window::close()
{
	DestroyWindow(handle);
}

void Win_Window::setVisible(bool visible)
{
	ShowWindow(handle, visible ? SW_SHOWNORMAL : SW_HIDE);
}

void Win_Window::pollEvents()
{
	AES_PROFILE_FUNCTION();
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

