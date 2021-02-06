#ifndef WINDOW_HPP
#define WINDOW_HPP

#include "aes.hpp"
#include "input.hpp"
#include <utility>

namespace aes {

	template<typename F>
	struct ContextCallback
	{
		operator bool() const noexcept
		{
			return fn != nullptr;
		}

		template<typename ...Args>
		auto operator()(Args&&... args)
		{
			AES_ASSERT(fn);
			return fn(std::forward<Args>(args)..., userData);
		}

		F fn = nullptr;
		void* userData = nullptr;
	};

	class Window
	{

	public:

		using ResizeCallbackT = void(*)(uint, uint);
		using KeyCallbackT = ContextCallback<void(*)(InputAction action, int key, void* userData)>;
		using MouseMoveCallbackT = void(*)(int mouseX, int mouseY);
		
		virtual ~Window() {};

		virtual void setResizeCallback(ResizeCallbackT func);
		virtual void setKeyCallback(KeyCallbackT func);
		virtual void setMouseMoveCallback(MouseMoveCallbackT func);
		virtual void getMousePosition(int& mouseX, int& mouseY) const;
		virtual void getScreenSize(uint& sizeX, uint& sizeY) const;
		virtual bool shouldClose() const;
		
		virtual void* getHandle() const = 0;
		virtual void open() = 0;
		virtual void setVisible(bool) = 0;
		virtual void close() = 0;
		virtual void pollEvents() = 0;

	protected:

		bool shouldClose_ = false;
		KeyCallbackT keyCallback;
		ResizeCallbackT resizeCallback = nullptr;
		MouseMoveCallbackT mouseMoveCallback = nullptr;
		uint width, height;
		int mouseX, mouseY;
	};

	class EmptyWindow : public Window
	{
	public:
		void* getHandle() const override
		{
			return nullptr;
		}
		void open() override {}
		void setVisible(bool) override {}
		void close() override {}
		void pollEvents() override {}
	};

}
#endif // !WINDOW_HPP
