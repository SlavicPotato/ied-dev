#pragma once

namespace IED
{
	enum class KeyEventType : std::uint8_t
	{
		Keyboard = 0,
		Mouse    = 1,
		Wheel    = 2
	};

	enum class KeyEventState : std::uint8_t
	{
		KeyUp   = 0,
		KeyDown = 1
	};

}