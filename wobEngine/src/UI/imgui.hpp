#ifndef AES_IMGUI_HPP
#define AES_IMGUI_HPP

#include "core/aes.hpp"
#include "core/color.hpp"
#include "core/allocator.hpp"
#include "core/array.hpp"
#include "core/geometry.hpp"
#include <variant>

/*
 * AES Object Oriented ImGui implementation, heavly inspired by Dear ImGui and microUI
 */

namespace aes
{
	using ImGuiId = int;
	constexpr ImGuiId invalidId = -1;

	struct ImguiWindow
	{
		String name;
		ImGuiId id;
	};

	enum class ImguiDrawCmdType
	{
		Text,
		Rect,
		FillRect,
		Image
	};

	struct TextArg
	{
		vec2 pos;
		String str;
	};

	struct RectArg
	{
		Rect rect;
		Color col;
	};

	struct ImguiDrawCmd
	{
		using Arg_t = std::variant<TextArg, RectArg>;

		ImguiDrawCmdType type;
		Arg_t arg;
	};

	struct InputData
	{
		vec2 cursorPos;
		bool cursorDown;
		bool cursorPressed;
	};

	class ImguiContext
	{
	public:
		
		void updateInputData(InputData const& inputData);
		void beginFrame();

		void beginWindow(const char* winName);
		void endWindow();

		void label(const char* text);
		bool button(const char* name);
		bool sliderFloat(const char* name, float& f);

	//private:

		ImGuiId hotItem, activeItem;
		float ny;
		InputData inputData;
		Array<ImguiWindow> windows;

		Array<ImguiDrawCmd> drawCmds;
	};
}

#endif
