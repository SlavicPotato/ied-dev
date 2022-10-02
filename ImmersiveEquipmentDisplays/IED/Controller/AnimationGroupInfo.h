#pragma once

#include "IED/AnimationWeaponType.h"

namespace IED
{
	enum class AnimationExtraGroup : std::uint32_t
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

		[[nodiscard]] inline constexpr std::int32_t get_value(
			AnimationWeaponType a_id,
			std::int32_t        a_value) const noexcept
		{
			assert(a_id < AnimationWeaponType::Max);

			auto base = data[stl::underlying(a_id)].base;

			return base > 0 ? base + a_value : 0;
		}

		[[nodiscard]] inline constexpr std::int32_t get_value_extra(
			AnimationExtraGroup a_id,
			std::int32_t        a_value) const noexcept
		{
			assert(a_id < AnimationExtraGroup::Max);

			auto base = data[stl::underlying(a_id)].base;

			return base > 0 ? base + a_value : 0;
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

		[[nodiscard]] inline constexpr void set_base_extra(
			AnimationExtraGroup a_id,
			std::int32_t        a_value) noexcept
		{
			assert(a_id < AnimationExtraGroup::Max);
			data[stl::underlying(a_id)].base = a_value;
		}

		std::int32_t                                                 crc{ 0 };
		std::array<Group, stl::underlying(AnimationWeaponType::Max)> data{};
		std::array<Group, stl::underlying(AnimationExtraGroup::Max)> extra{};
	};

}