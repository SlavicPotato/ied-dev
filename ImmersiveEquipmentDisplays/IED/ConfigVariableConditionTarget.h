#pragma once

namespace IED
{
	namespace Data
	{
		enum class VariableConditionTarget : std::uint32_t
		{
			kAll   = 0,
			kSelf  = 1,
			kActor = 2,
			kNPC   = 3,
			kRace  = 4,
		};
	}
}