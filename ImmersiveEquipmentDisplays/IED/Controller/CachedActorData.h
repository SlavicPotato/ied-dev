#pragma once

//#include "IED/ConfigCommon.h"

namespace IED
{
	inline static constexpr auto ACTOR_CHECK_FLAGS_1 =
		Actor::Flags1::kPlayerTeammate |
		Actor::Flags1::kGuard |
		Actor::Flags1::kParalyzed;

	inline static constexpr auto ACTOR_CHECK_FLAGS_2 =
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

	inline static constexpr auto ACTOR_CHECK_FLAGS_LF_1 =
		Actor::Flags1::kInWater;

	inline static constexpr auto ACTOR_CHECK_FLAGS_LF_2 =
		Actor::Flags2::kUnderwater;

	class CachedFactionData
	{
		using container_type =
			stl::map_sa<
				TESFaction*,
				std::int8_t>;

	public:
		CachedFactionData(Actor* a_actor);

		bool UpdateFactions(Actor* a_actor);

		[[nodiscard]] inline constexpr auto GetNumFactions() const noexcept
		{
			return data.size();
		}

		inline constexpr const auto& GetFactionContainer() const noexcept
		{
			return data;
		}

	private:
		template <class Tf>
		void visit_factions(
			const ExtraFactionChanges* a_factionChanges,
			TESNPC*                    a_npc,
			Tf                         a_func);

		std::size_t GetSignature(const ExtraFactionChanges* a_factionChanges, TESNPC* a_npc);

		container_type data;

		std::size_t currentSignature{ hash::fnv1::fnv_offset_basis };
	};

	template <class Tf>
	void CachedFactionData::visit_factions(
		const ExtraFactionChanges* a_factionChanges,
		TESNPC*                    a_npc,
		Tf                         a_func)
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
		using container_type =
			stl::set_sa<EffectSetting*>;

	public:
		CachedActiveEffectData(Actor* a_actor);

		bool UpdateEffects(Actor* a_actor);

		inline constexpr const auto& GetEffectContainer() const noexcept
		{
			return data;
		}

		bool HasEffectWithKeyword(const BGSKeyword* a_keyword) const;

	private:
		template <class Tf>
		void visit_effects(
			RE::BSSimpleList<ActiveEffect*>* a_list,
			Tf                               a_func);

		std::size_t GetSignature(RE::BSSimpleList<ActiveEffect*>* a_list);

		container_type data;

		std::size_t currentSignature{ hash::fnv1::fnv_offset_basis };
	};

	template <class Tf>
	void CachedActiveEffectData::visit_effects(
		RE::BSSimpleList<ActiveEffect*>* a_list,
		Tf                               a_func)
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
				a_func(mgef);
			}
		}
	}

	struct CachedActorData :
		CachedFactionData,
		CachedActiveEffectData
	{
		CachedActorData(Actor* a_actor);

		bool UpdateState(Actor* a_actor, TESObjectCELL* a_cell);
		bool UpdateStateLF(Actor* a_actor);
		bool UpdateStateHF(Actor* a_actor);

		TESWorldSpace*           worldspace{ nullptr };
		TESPackage*              currentPackage{ nullptr };
		TESIdleForm*             currentIdle{ nullptr };
		stl::flag<Actor::Flags1> flags1{ Actor::Flags1::kNone };
		stl::flag<Actor::Flags2> flags2{ Actor::Flags2::kNone };
		stl::flag<Actor::Flags1> flagslf1{ Actor::Flags1::kNone };
		stl::flag<Actor::Flags2> flagslf2{ Actor::Flags2::kNone };
		bool                     cellAttached{ false };
		bool                     inCombat{ false };
		bool                     swimming{ false };
		bool                     sitting{ false };
		bool                     sleeping{ false };
		bool                     beingRidden{ false };
		bool                     weaponDrawn{ false };
		bool                     inInterior{ false };
		bool                     arrested{ false };
		bool                     unconscious{ false };
		bool                     flying{ false };
	};
}