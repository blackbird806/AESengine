#include "UI.hpp"

static bool pointInRectangle(int px, int py, int x, int y, int w, int h)
{
	if (px < x ||
		py < y ||
		px >= x + w ||
		py >= y + h)
		return false;
	return true;
}

aes::ui::ImguiContext::ImguiContext(std::pmr::memory_resource* allocator) : commandList(allocator)
{
	AES_ASSERT(allocator);
}

void aes::ui::ImguiContext::startFrame()
{
	commandList.clear();
	activeItem = invalidId;
	hotItem = invalidId;
}

auto aes::ui::ImguiContext::regionHit(int x, int y, int w, int h) const -> bool
{
	return pointInRectangle(mouse_x, mouse_y, x, y, w, h);
}

bool aes::ui::ImguiContext::button(int id, int x, int y)
{
	Color col(0, 255, 255, 120);

	if (regionHit(x, y, 64, 48))
	{
		hotItem = id;
		if (activeItem == 0 && cursor_down)
			activeItem = id;
		col.g = 0;
	}

	commandList.push_back(Command{col, Rect{x, y, 64, 48}});

	// If button is hot and active, but mouse button is not
	// down, the user must have clicked the button.
	return (cursor_down && hotItem == id && activeItem == id);
}

std::pmr::vector<aes::ui::Command> const& aes::ui::ImguiContext::getCommandList() const
{
	return commandList;
}
