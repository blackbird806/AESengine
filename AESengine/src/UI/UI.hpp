#ifndef UI_HPP
#define UI_HPP

#include <variant>
#include <vector>
#include <memory_resource>

#include "core/aes.hpp"
#include "core/color.hpp"

namespace aes::ui
{
	struct Rect
	{
		int x, y, w, h;
	};

	struct Line
	{
		int x1, y1, x2, y2;
	};

	struct Text
	{
		std::string txt;
		int x, y, size;
	};
	
	struct Command
	{
		using Type = std::variant<Rect, Line, Text>;
		
		Color color;
		Type type;
	};
	
	struct ImguiContext
	{
		static constexpr int invalidId = -1;
		
		int mouse_x, mouse_y;
		int hotItem, activeItem;
		bool cursor_down = false;
		
		std::pmr::vector<Command> commandList;

		explicit ImguiContext(std::pmr::memory_resource* allocator);
		
		void startFrame();
		
		bool regionHit(int x, int y, int w, int h) const;
		bool button(int id, int x, int y);

		std::pmr::vector<Command> const& getCommandList() const;
	};
}

#endif
