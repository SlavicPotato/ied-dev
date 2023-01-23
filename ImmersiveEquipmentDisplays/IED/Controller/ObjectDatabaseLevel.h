#pragma once

namespace IED
{
	enum class ObjectDatabaseLevel : std::uint32_t
	{
		kDisabled = static_cast<std::underlying_type_t<ObjectDatabaseLevel>>(-1), // not used

		kNone     = 0,
		kMin      = 2,
		kVeryLow  = 5,
		kLow      = 10,
		kMedium   = 15,
		kHigh     = 20,
		kVeryHigh = 25,
		kExtreme  = 35,
		kMax      = 50
	};
}