#pragma once

//#include "IED/ConfigCommon.h"

namespace IED
{
	static constexpr auto ACTOR_CHECK_FLAGS_1 =
		Actor::Flags1::kPlayerTeammate |
		Actor::Flags1::kGuard |
		Actor::Flags1::kParalyzed;

	static constexpr auto ACTOR_CHECK_FLAGS_2 =
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

	static constexpr auto ACTOR_CHECK_FLAGS_LF_1 =
		Actor::Flags1::kInWater;

	static constexpr auto ACTOR_CHECK_FLAGS_LF_2 =
		Actor::Flags2::kUnderwater;

	static constexpr auto NPC_BASE_CHECK_FLAGS =
		ACTOR_BASE_DATA::Flag::kInvulnerable |
		ACTOR_BASE_DATA::Flag::kSummonable |
		ACTOR_BASE_DATA::Flag::kUnique;

	class CachedPerkData
	{
		using container_type =
			stl::cache_aligned::flat_map<
				Game::FormID,
				std::int8_t>;

	public:
		CachedPerkData(Actor* a_actor) noexcept;

		bool UpdatePerks(Actor* a_actor, TESNPC* a_npc) noexcept;

		[[nodiscard]] constexpr auto GetNumPerks() const noexcept
		{
			return data.size();
		}

		[[nodiscard]] constexpr const auto& GetPerkContainer() const noexcept
		{
			return data;
		}

	private:
		[[nodiscard]] static constexpr std::size_t GetSignature(Actor* a_actor) noexcept;

		container_type data;

		std::size_t currentSignature{ 0 };
	};

	class CachedFactionData
	{
		using container_type =
			stl::cache_aligned::flat_map<
				TESFaction*,
				std::int8_t>;

	public:
		CachedFactionData(Actor* a_actor) noexcept;

		bool UpdateFactions(Actor* a_actor, TESNPC* a_npc) noexcept;

		[[nodiscard]] constexpr auto GetNumFactions() const noexcept
		{
			return data.size();
		}

		[[nodiscard]] constexpr const auto& GetFactionContainer() const noexcept
		{
			return data;
		}

	private:
		template <class Tf>
		static constexpr void visit_factions(
			const ExtraFactionChanges* a_factionChanges,
			TESNPC*                    a_npc,
			Tf                         a_func)                                                          //
			noexcept(std::is_nothrow_invocable_v<Tf, const RE::FACTION_RANK&>)  //
			requires(std::invocable<Tf, const RE::FACTION_RANK&>);

		[[nodiscard]] static constexpr std::size_t GetSignature(
			const ExtraFactionChanges* a_factionChanges,
			TESNPC*                    a_npc) noexcept;

		container_type data;

		std::size_t currentSignature{ 0 };
	};

	template <class Tf>
	constexpr void CachedFactionData::visit_factions(
		const ExtraFactionChanges* a_factionChanges,
		TESNPC*                    a_npc,
		Tf                         a_func)                                                          //
		noexcept(std::is_nothrow_invocable_v<Tf, const RE::FACTION_RANK&>)  //
		requires(std::invocable<Tf, const RE::FACTION_RANK&>)
	{
		if (a_factionChanges)
		{
			for (const auto& info : a_factionChanges->factions)
			{
				if (info.faction)
				{
					a_func(info);
				}
			}
		}

		for (const auto& info : a_npc->factions)
		{
			if (info.faction)
			{
				a_func(info);
			}
		}
	}

	class CachedActiveEffectData
	{
		using container_type = stl::cache_aligned::flat_set<EffectSetting*>;

	public:
		CachedActiveEffectData(Actor* a_actor) noexcept;

		bool UpdateEffects(Actor* a_actor) noexcept;

		constexpr auto& GetEffectContainer() const noexcept
		{
			return data;
		}

		bool HasEffectWithKeyword(const BGSKeyword* a_keyword) const noexcept;

	private:
		template <class Tf>
		static constexpr void visit_effects(
			RE::BSSimpleList<ActiveEffect*>* a_list,
			Tf                               a_func)                                                                //
			noexcept(std::is_nothrow_invocable_v<Tf, ActiveEffect*, EffectSetting*>)  //
			requires(std::invocable<Tf, ActiveEffect*, EffectSetting*>);

		static constexpr std::size_t GetSignature(
			RE::BSSimpleList<ActiveEffect*>* a_list) noexcept;

		container_type data;

		std::size_t currentSignature{ hash::fnv1::fnv_offset_basis };
	};

	template <class Tf>
	constexpr void CachedActiveEffectData::visit_effects(
		RE::BSSimpleList<ActiveEffect*>* a_list,
		Tf                               a_func)                                                                //
		noexcept(std::is_nothrow_invocable_v<Tf, ActiveEffect*, EffectSetting*>)  //
		requires(std::invocable<Tf, ActiveEffect*, EffectSetting*>)
	{
		if (!a_list)
		{
			return;
		}

		for (auto e : *a_list)
		{
			if (!e)
			{
				continue;
			}

			auto effect = e->effect;
			if (!effect)
			{
				continue;
			}

			auto mgef = effect->mgef;
			if (!mgef)
			{
				continue;
			}

			if (!mgef->formID.IsTemporary())
			{
				a_func(e, mgef);
			}
		}
	}

	struct CachedActorData :
		CachedFactionData,
		CachedActiveEffectData,
		CachedPerkData
	{
		CachedActorData(Actor* a_actor) noexcept;

		bool UpdateState(const Actor* a_actor, const TESObjectCELL* a_cell) noexcept;
		bool UpdateStateLF(const Actor* a_actor) noexcept;
		bool UpdateStateHF(const Actor* a_actor) noexcept;
		bool DoLFUpdates(Actor* a_actor) noexcept;

		TESWorldSpace*                   worldspace;
		TESPackage*                      currentPackage;
		TESIdleForm*                     currentIdle;
		stl::flag<Actor::Flags1>         flags1;
		stl::flag<Actor::Flags2>         flags2;
		stl::flag<Actor::Flags1>         flagslf1;
		stl::flag<Actor::Flags2>         flagslf2;
		stl::flag<ACTOR_BASE_DATA::Flag> baseFlags;
		ActorState::ACTOR_LIFE_STATE     lifeState;
		std::uint16_t                    level;
		bool                             active;
		bool                             inCombat;
		bool                             swimming;
		bool                             sitting;
		bool                             sleeping;
		bool                             beingRidden;
		bool                             weaponDrawn;
		bool                             inInterior;
		bool                             arrested;
		bool                             flying;
		bool                             sneaking;
	};
}