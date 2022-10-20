#pragma once

namespace IED
{
	namespace Data
	{
		enum class VariableConditionSource : std::uint32_t
		{
			kAny         = 0,
			kSelf        = 1,
			kActor       = 2,
			kNPC         = 3,
			kRace        = 4,
			kPlayerHorse = 5,
		};
	}
}