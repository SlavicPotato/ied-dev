#pragma once

namespace IED
{
	enum class GearNodeID : std::uint32_t
	{
		kNone = 0,

		k1HSword              = 1,
		k1HSwordLeft          = 2,
		k1HAxe                = 3,
		k1HAxeLeft            = 4,
		kTwoHanded            = 5,
		kTwoHandedAxeMace     = 6,
		kDagger               = 7,
		kDaggerLeft           = 8,
		kMace                 = 9,
		kMaceLeft             = 10,
		kStaff                = 11,
		kStaffLeft            = 12,
		kBow                  = 13,
		kCrossBow             = 14,
		kShield               = 15,
		kQuiver               = 16,
		kTwoHandedLeft        = 17,
		kTwoHandedAxeMaceLeft = 18
	};

}