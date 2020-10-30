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