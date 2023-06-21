#ifndef AES_IMGUI_HPP
#define AES_IMGUI_HPP

#include "core/aes.hpp"
#include "core/color.hpp"
#include "core/allocator.hpp"
#include "core/array.hpp"
#include "core/geometry.hpp"
#include "core/stringView.hpp"

/*
 * AES Object Oriented ImGui implementation, heavly inspired by Dear ImGui and microUI
 */
namespace aes
{
	using ImGuiId = int;
	constexpr ImGuiId invalidId = -1;

	struct ImguiWindow
	{
		std::string name;
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
		glm::vec2 pos;
		std::string str;
	};

	struct RectArg
	{
		Rect rect;
		Color col;
	};

	struct ImguiDrawCmd
	{
		//using Arg_t = std::variant<TextArg, RectArg>;

		ImguiDrawCmdType type;
		//Arg_t arg;
	};

	struct InputData
	{
		glm::vec2 cursorPos;
		bool cursorDown;
		bool cursorPressed;
	};

	class ImguiContext
	{
	public:
		
		explicit ImguiContext(IAllocator& allocator);

		void updateInputData(InputData const& inputData);
		void beginFrame();

		void beginWindow(StringView winName);
		void endWindow();

		void label(StringView text);
		bool button(StringView name);
		bool sliderFloat(StringView name, float& f);

	//private:

		ImGuiId hotItem, activeItem;
		float ny;
		InputData inputData;
		Array<ImguiWindow> windows;

		Array<ImguiDrawCmd> drawCmds;
	};
}

#endif
