#ifndef WOB_WIN_WINDOW_HPP
#define WOB_WIN_WINDOW_HPP

#include "core/wob.hpp"
#include "core/os.hpp"
#include "core/window.hpp"

namespace wob {

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
