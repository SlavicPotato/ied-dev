#pragma once

namespace IED
{
	enum class AnimationWeaponSlot : std::uint32_t
	{
		None = static_cast<std::underlying_type_t<AnimationWeaponSlot>>(-1),

		Sword = 0,
		SwordLeft,
		Axe,
		AxeLeft,
		Dagger,
		DaggerLeft,
		Mace,
		MaceLeft,
		TwoHanded,
		Quiver,

		Max
	};

}