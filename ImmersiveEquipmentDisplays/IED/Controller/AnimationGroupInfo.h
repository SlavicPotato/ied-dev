#pragma once

#include "AnimationWeaponType.h"

namespace IED
{
	struct AnimationGroupInfo
	{
		struct Group
		{
			std::int32_t base{ 0 };
		};

		[[nodiscard]] inline constexpr std::int32_t get_value(
			AnimationWeaponType a_id,
			std::int32_t        a_value) const noexcept
		{
			assert(a_id < AnimationWeaponType::Max);

			return a_value < 0 ?
			           0 :
                       data[stl::underlying(a_id)].base + a_value;
		}
		
		[[nodiscard]] inline constexpr std::int32_t get_base(
			AnimationWeaponType a_id) const noexcept
		{
			assert(a_id < AnimationWeaponType::Max);
			return data[stl::underlying(a_id)].base;
		}

		[[nodiscard]] inline constexpr void set_base(
			AnimationWeaponType a_id,
			std::int32_t        a_value) noexcept
		{
			assert(a_id < AnimationWeaponType::Max);
			data[stl::underlying(a_id)].base = a_value;
		}

		std::int32_t crc{ 0 };
		Group        data[stl::underlying(AnimationWeaponType::Max)];
	};

}