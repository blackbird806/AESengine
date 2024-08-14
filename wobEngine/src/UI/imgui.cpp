#include "imgui.hpp"

using namespace aes;

void ImguiContext::updateInputData(InputData const& in)
{
	inputData = in;
}

void ImguiContext::beginFrame()
{
	drawCmds.clear();
	ny = 0.9f;
}

void ImguiContext::beginWindow(const char* winName)
{
}

void ImguiContext::endWindow()
{
}

void ImguiContext::label(const char* text)
{
	drawCmds.push({ ImguiDrawCmdType::Text, TextArg{vec2(-1, ny), String(text)}});
	ny -= 0.05f;
}

bool ImguiContext::button(const char* name)
{
	Rect const btnRect{ vec2(-1, ny), vec2(-0.8, ny + 0.05) };

	bool ret = false;
	if (pointInRect(inputData.cursorPos, btnRect))
	{
		ret = inputData.cursorPressed;
		drawCmds.push({ ImguiDrawCmdType::FillRect, RectArg{btnRect, Color(255, 0, 0)}});
	}
	else
	{
		drawCmds.push({ ImguiDrawCmdType::FillRect, RectArg{btnRect, Color(0, 0, 255)} });
	}

	drawCmds.push({ ImguiDrawCmdType::Text, TextArg{vec2(-1, ny), String(name)} });
	ny -= 0.05f;
	return ret;
}

bool ImguiContext::sliderFloat(const char* name, float& f)
{
	float const sliderStart = -0.5, sliderEnd = 0.0;
	float const sliderRange = sliderEnd - sliderStart;
	Rect const sliderRect{ vec2(sliderStart, ny), vec2(sliderEnd, ny + 0.04) };
	Rect const sliderCursor{ vec2(sliderStart + f * sliderRange, ny), vec2(sliderStart + f * sliderRange + 0.05, ny + 0.04) };

	drawCmds.push({ ImguiDrawCmdType::Text, TextArg{vec2(-1, ny), String(name)} });
	drawCmds.push({ ImguiDrawCmdType::FillRect, RectArg{sliderRect, Color(50, 50, 50)} });
	drawCmds.push({ ImguiDrawCmdType::FillRect, RectArg{sliderCursor, Color(127, 127, 127)} });
	ny -= 0.05f;

	if (pointInRect(inputData.cursorPos, sliderCursor) && inputData.cursorDown)
	{
		float const npos = inputData.cursorPos.x;
		f = (npos - sliderStart) ;
		return true;
	}
	return false;
}

