#pragma once

namespace IED
{
	enum class BackgroundCloneLevel : std::uint8_t
	{
		kNone    = 0,
		kTexSwap = 1,
		kClone   = 2
	};
}