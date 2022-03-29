#pragma once

#include "ActorAnimationState.h"
#include "AnimationGroupInfo.h"
#include "AnimationWeaponSlot.h"
#include "AnimationWeaponType.h"

namespace IED
{
	class IAnimationManager
	{
		inline static constexpr auto FNIS_AA2_PEX_PATH = "Data\\Scripts\\FNIS_aa2.pex";

		struct AnimStringHolder
		{
			AnimStringHolder();

			struct EquipEntry
			{
				BSFixedString name;
				BSFixedString crc;
			};

			[[nodiscard]] inline constexpr auto& get_eqp(AnimationWeaponType a_id) noexcept
			{
				assert(a_id < AnimationWeaponType::Max);
				return eqp[stl::underlying(a_id)];
			}

			[[nodiscard]] inline constexpr auto& get_eqp(AnimationWeaponType a_id) const noexcept
			{
				assert(a_id < AnimationWeaponType::Max);
				return eqp[stl::underlying(a_id)];
			}

			EquipEntry    eqp[stl::underlying(AnimationWeaponType::Max)]{};
			BSFixedString FNISaa_crc{ "FNISaa_crc" };
		};

	protected:
		void        InitializeAnimationStrings();
		static void ExtractAnimationInfoFromPEX(AnimationGroupInfo& a_out);

		void SetAnimationInfo(const AnimationGroupInfo& a_in);

		void UpdateAA(Actor* a_actor, ActorAnimationState& a_state);
		void ResetAA(Actor* a_actor, ActorAnimationState& a_state);

	private:
		void SetAnimationVar(
			Actor*               a_actor,
			ActorAnimationState& a_state,
			AnimationWeaponType  a_id,
			std::int32_t         a_value);

		AnimationWeaponType GetObjectType(TESForm* a_object);

		std::optional<AnimationGroupInfo>       m_groupInfo;
		std::unique_ptr<const AnimStringHolder> m_strings;
	};
}