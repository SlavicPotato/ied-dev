#pragma once

namespace IED
{
	enum class KeyEventType : std::uint32_t
	{
		Keyboard = 0,
		Mouse = 1,
		Wheel = 2
	};

	enum class KeyEventState : std::uint32_t
	{
		KeyDown = 0,
		KeyUp = 1
	};

}