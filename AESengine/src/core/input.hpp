#ifndef INPUT_HPP
#define INPUT_HPP

namespace aes
{
	enum class Key
	{
		W,
		A,
		S, 
		D, 
		R, 
		E,
		Q,
		T,
		Up, 
		Down,
		Left,
		Right,
		Space,

		LClick,
		RClick,
		MClick,

		Num0,
		Num1,
		Num2,
		Num3,
		Num4,
		Num5,
		Num6,
		Num7,
		Num8,
		Num9,
		
		None,
	};

	enum class InputAction {
		Pressed,
		Released
	};

	enum class InputState {
		Up,
		Down,
	};

	Key windowsToAESKey(int winKey);
}

#endif