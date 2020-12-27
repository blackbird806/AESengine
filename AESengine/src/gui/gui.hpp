#ifndef GUI_HPP
#define GUI_HPP

#include <string_view>
#include <vector>

#include "core/aes.hpp"
#include "core/geometry.hpp"

#include <glm/glm.hpp>

/*
 * Immediate mode gui
 */

namespace aes {
	namespace gui
	{
		struct InputState
		{
			
		};

		struct Layout
		{
			Rect body;
			
		};

		struct Window
		{
			int id;
			Layout layout;
		};

		class Context
		{
			void beginWindow(std::string_view name);
			bool checkBox(std::string_view label, bool& value);
			bool button(std::string_view label);
			void endWindow();

			int focusedId = -1;
			Layout rootLayout;
			InputState inputState;

			int nextElementId = 0;
		};
	}
}

#endif