#include "imgui.hpp"

using namespace aes;

ImguiContext::ImguiContext(IAllocator& allocator) : windows(allocator), drawCmds(allocator)
{

}

void ImguiContext::updateInputData(InputData const& in)
{
	inputData = in;
}

void ImguiContext::beginFrame()
{
	drawCmds.clear();
	ny = 0.9f;
}

void ImguiContext::beginWindow(StringView winName)
{
}

void ImguiContext::endWindow()
{
}

void ImguiContext::label(StringView text)
{
	//drawCmds.push({ ImguiDrawCmdType::Text, TextArg{glm::vec2(-1, ny), std::string(text)}});
	ny -= 0.05f;
}

bool ImguiContext::button(StringView name)
{
	Rect const btnRect{ glm::vec2(-1, ny), glm::vec2(-0.8, ny + 0.05) };

	bool ret = false;
	if (pointInRect(inputData.cursorPos, btnRect))
	{
		ret = inputData.cursorPressed;
		//drawCmds.push({ ImguiDrawCmdType::FillRect, RectArg{btnRect, Color(255, 0, 0)}});
	}
	else
	{
		//drawCmds.push({ ImguiDrawCmdType::FillRect, RectArg{btnRect, Color(0, 0, 255)} });
	}

	//drawCmds.push({ ImguiDrawCmdType::Text, TextArg{glm::vec2(-1, ny), std::string(name)} });
	ny -= 0.05f;
	return ret;
}

bool ImguiContext::sliderFloat(StringView name, float& f)
{
	float const sliderStart = -0.5, sliderEnd = 0.0;
	float const sliderRange = sliderEnd - sliderStart;
	Rect const sliderRect{ glm::vec2(sliderStart, ny), glm::vec2(sliderEnd, ny + 0.04) };
	Rect const sliderCursor{ glm::vec2(sliderStart + f * sliderRange, ny), glm::vec2(sliderStart + f * sliderRange + 0.05, ny + 0.04) };

	//drawCmds.push({ ImguiDrawCmdType::Text, TextArg{glm::vec2(-1, ny), std::string(name)} });
	//drawCmds.push({ ImguiDrawCmdType::FillRect, RectArg{sliderRect, Color(50, 50, 50)} });
	//drawCmds.push({ ImguiDrawCmdType::FillRect, RectArg{sliderCursor, Color(127, 127, 127)} });
	ny -= 0.05f;

	if (pointInRect(inputData.cursorPos, sliderCursor) && inputData.cursorDown)
	{
		float const npos = inputData.cursorPos.x;
		f = (npos - sliderStart) ;
		return true;
	}
	return false;
}

