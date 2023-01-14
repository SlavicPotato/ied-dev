#pragma once

#include "IED/AnimationWeaponType.h"

namespace IED
{
	enum class AnimationExtraGroup : std::uint8_t
	{
		BowIdle = 0,
		BowAttack,

		Max
	};

	struct AnimationGroupInfo
	{
		struct Group
		{
			std::int32_t base{ 0 };
		};

		[[nodiscard]] constexpr std::int32_t get_value(
			AnimationWeaponType a_id,
			std::int32_t        a_value) const noexcept
		{
			const auto base = get_base(a_id);

			return base > 0 ? base + a_value : 0;
		}

		[[nodiscard]] constexpr std::int32_t get_value_extra(
			AnimationExtraGroup a_id,
			std::int32_t        a_value) const noexcept
		{
			const auto base = get_base_extra(a_id);

			return base > 0 ? base + a_value : 0;
		}

		[[nodiscard]] constexpr std::int32_t get_base(
			AnimationWeaponType a_id) const noexcept
		{
			assert(a_id < AnimationWeaponType::Max);
			return data[stl::underlying(a_id)].base;
		}

		[[nodiscard]] constexpr std::int32_t get_base_extra(
			AnimationExtraGroup a_id) const noexcept
		{
			assert(a_id < AnimationExtraGroup::Max);
			return extra[stl::underlying(a_id)].base;
		}

		constexpr void set_base(
			AnimationWeaponType a_id,
			std::int32_t        a_value) noexcept
		{
			assert(a_id < AnimationWeaponType::Max);
			data[stl::underlying(a_id)].base = a_value;
		}

		constexpr void set_base_extra(
			AnimationExtraGroup a_id,
			std::int32_t        a_value) noexcept
		{
			assert(a_id < AnimationExtraGroup::Max);
			extra[stl::underlying(a_id)].base = a_value;
		}

		std::int32_t                                                 crc{ 0 };
		std::array<Group, stl::underlying(AnimationWeaponType::Max)> data;
		std::array<Group, stl::underlying(AnimationExtraGroup::Max)> extra;
	};

}