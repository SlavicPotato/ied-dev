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

			std::array<EquipEntry, stl::underlying(AnimationWeaponType::Max)> eqp;
			BSFixedString                                                     FNISaa_crc{ "FNISaa_crc" };
		};

	public:
		enum class PresenceFlags : std::uint32_t
		{
			kNone = 0,

			kSword   = 1u << 0,
			kAxe     = 1u << 1,
			kDagger  = 1u << 2,
			kMace    = 1u << 3,
			k2hSword = 1u << 4,
			k2hAxe   = 1u << 5,
			kBow     = 1u << 6,

			kAll = kSword |
			       kAxe |
			       kDagger |
			       kMace |
			       k2hSword |
			       k2hAxe |
			       kBow
		};

	protected:
		void                      InitializeAnimationStrings();
		static AnimationGroupInfo ExtractAnimationInfoFromPEX();

		inline constexpr void SetAnimationInfo(const AnimationGroupInfo& a_in) noexcept(
			std::is_nothrow_constructible_v<AnimationGroupInfo>)
		{
			m_groupInfo.emplace(a_in);
		}

		[[nodiscard]] inline constexpr auto& GetAnimationInfo() const noexcept
		{
			return m_groupInfo;
		}

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

	DEFINE_ENUM_CLASS_BITWISE(IAnimationManager::PresenceFlags);
}