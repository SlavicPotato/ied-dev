#pragma once

#include "IED/AnimationWeaponSlot.h"
#include "IED/AnimationWeaponType.h"

#include "IED/NodeOverrideData.h"

namespace IED
{
	struct ActorAnimationState
	{
		enum class Flags : std::uint32_t
		{
			kNone = 0,

			kNeedUpdate = 1u << 0
		};

		struct Entry
		{
			std::int32_t animVar{ -1 };
		};

		[[nodiscard]] inline constexpr auto& get(AnimationWeaponType a_id) noexcept
		{
			assert(a_id < AnimationWeaponType::Max);
			return data[stl::underlying(a_id)];
		}

		[[nodiscard]] inline constexpr auto& get_placement(AnimationWeaponSlot a_id) const noexcept
		{
			assert(a_id < AnimationWeaponSlot::Max);
			return placement[stl::underlying(a_id)];
		}

		stl::flag<Flags> flags{ Flags::kNone };

		std::array<Entry, stl::underlying(AnimationWeaponType::Max)>             data{};
		std::array<WeaponPlacementID, stl::underlying(AnimationWeaponSlot::Max)> placement{ WeaponPlacementID::None };

		//std::pair<TESForm*, TESForm*> equipped{ nullptr, nullptr };
	};

	DEFINE_ENUM_CLASS_BITWISE(ActorAnimationState::Flags);
}