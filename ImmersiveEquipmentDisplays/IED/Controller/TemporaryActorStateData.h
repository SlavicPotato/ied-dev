#pragma once

namespace IED
{
	inline static constexpr Actor::Flags1 ACTOR_CHECK_FLAGS_1 =
		Actor::Flags1::kPlayerTeammate |
		Actor::Flags1::kGuard |
		Actor::Flags1::kParalyzed;

	inline static constexpr Actor::Flags2 ACTOR_CHECK_FLAGS_2 =
		Actor::Flags2::kIsAMount |
		Actor::Flags2::kGettingOnOffMount |
		Actor::Flags2::kInBleedoutAnimation |
		Actor::Flags2::kIsTrespassing |
		Actor::Flags2::kIsCommandedActor |
		Actor::Flags2::kBribedByPlayer |
		Actor::Flags2::kAngryWithPlayer |
		Actor::Flags2::kEssential |
		Actor::Flags2::kProtected |
		Actor::Flags2::kIsInKillMove;

	inline static constexpr Actor::Flags1 ACTOR_CHECK_FLAGS_LF_1 =
		Actor::Flags1::kInWater;

	inline static constexpr Actor::Flags2 ACTOR_CHECK_FLAGS_LF_2 =
		Actor::Flags2::kUnderwater;

	struct TemporaryActorStateData
	{
		TemporaryActorStateData(Actor* a_actor);

		TESWorldSpace* worldspace{ nullptr };
		TESPackage*    currentPackage{ nullptr };
		TESIdleForm*   currentIdle{ nullptr };
		Actor::Flags1  flags1{ Actor::Flags1::kNone };
		Actor::Flags2  flags2{ Actor::Flags2::kNone };
		Actor::Flags1  flagslf1{ Actor::Flags1::kNone };
		Actor::Flags2  flagslf2{ Actor::Flags2::kNone };
		bool           cellAttached{ false };
		bool           inCombat{ false };
		bool           swimming{ false };
		bool           sitting{ false };
		bool           sleeping{ false };
		bool           beingRidden{ false };
		bool           weaponDrawn{ false };
		bool           inInterior{ false };
		bool           arrested{ false };
	};
}