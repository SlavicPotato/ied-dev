#pragma once

namespace IED
{
	enum class I3DIModelID : std::uint32_t
	{
		kSphere,
		kAnchor,

		kOneHandedSword,
		kOneHandedAxe,
		kMace,
		kDagger,
		kTwoHanded,
		kBow,
		kStaff,
		kQuiver,
		kShield,

		kMax
	};
}