#ifndef WIN_WINDOW_HPP
#define WIN_WINDOW_HPP

#include "core/aes.hpp"
#include "core/os.hpp"
#include "core/window.hpp"

namespace aes {

	class Win_Window final : public Window
	{

	public:

		static LRESULT CALLBACK windowProcess(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
		
		explicit Win_Window(const char* name);
		~Win_Window() override;

		void* getHandle() const override;

		void open() override;
		void setVisible(bool) override;
		void close() override;
		void pollEvents() override;

	private:

		HWND handle;
	};

}
#endif // !WINDOW_HPP
