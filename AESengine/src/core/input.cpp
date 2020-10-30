#include "input.hpp"
#include "debug.hpp"
using namespace aes;

Key aes::windowsToAESKey(int winKey)
{
	// https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	switch (winKey)
	{
	case 0x01: return Key::LClick;
	case 0x02: return Key::RClick;
	case 0x04: return Key::MClick;
	case 0x20: return Key::Space;
	case 0x26: return Key::Up;
	case 0x27: return Key::Right;
	case 0x28: return Key::Down;
	case 0x29: return Key::Left;
	case 0x41: return Key::A;
	case 0x44: return Key::D;
	case 0x45: return Key::E;
	case 0x51: return Key::Q;
	case 0x52: return Key::R;
	case 0x53: return Key::S;
	case 0x54: return Key::T;
	case 0x57: return Key::W;
	default:
		return Key::None; // unsuported winkey
	}
}
