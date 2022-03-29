#pragma once

namespace IED
{
	enum class AnimationWeaponType : std::uint32_t
	{
		None = static_cast<std::underlying_type_t<AnimationWeaponType>>(-1),

		Sword = 0,
		Axe,
		Dagger,
		Mace,

		Max
	};

}