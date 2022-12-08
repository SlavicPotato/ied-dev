#include "pch.h"

#include "CachedActorData.h"

namespace IED
{

	CachedFactionData::CachedFactionData(Actor* a_actor)
	{
		UpdateFactions(a_actor);
	}

	bool CachedFactionData::UpdateFactions(Actor* a_actor)
	{
		auto npc = a_actor->GetActorBase();
		if (!npc)
		{
			return false;
		}

		const auto* extraFactionChanges = a_actor->extraData.Get<ExtraFactionChanges>();

		const auto sig = GetSignature(extraFactionChanges, npc);

		if (sig == currentSignature)
		{
			return false;
		}

		currentSignature = sig;

		data.clear();

		visit_factions(
			extraFactionChanges,
			npc,
			[&](auto& a_info) {
				data.emplace(a_info.faction, a_info.rank);
			});

		return true;
	}

	std::size_t CachedFactionData::GetSignature(
		const ExtraFactionChanges* a_factionChanges,
		TESNPC*                    a_npc)
	{
		auto result = hash::fnv1::fnv_offset_basis;

		visit_factions(
			a_factionChanges,
			a_npc,
			[&](const auto& a_info) [[msvc::forceinline]] {
				result = hash::fnv1::_append_hash_fnv1a(result, a_info.faction->formID);
				result = hash::fnv1::_append_hash_fnv1a(result, a_info.rank);
			});

		return result;
	}

	CachedActiveEffectData::CachedActiveEffectData(Actor* a_actor)
	{
		UpdateEffects(a_actor);
	}

	bool CachedActiveEffectData::UpdateEffects(Actor* a_actor)
	{
		auto list = a_actor->GetActiveEffectList();

		const auto sig = GetSignature(list);

		if (sig == currentSignature)
		{
			return false;
		}

		currentSignature = sig;

		data.clear();

		visit_effects(
			list,
			[&](auto* a_effect, auto* a_mgef) [[msvc::forceinline]] {
				if (!a_effect->flags.test_any(
						ActiveEffect::Flag::kDispelled |
						ActiveEffect::Flag::kInactive))
				{
					data.emplace(a_mgef);
				}
			});

		return true;
	}

	bool CachedActiveEffectData::HasEffectWithKeyword(const BGSKeyword* a_keyword) const
	{
		if (a_keyword)
		{
			for (auto& e : data)
			{
				if (e->HasKeyword(a_keyword))
				{
					return true;
				}
			}
		}

		return false;
	}

	std::size_t CachedActiveEffectData::GetSignature(
		RE::BSSimpleList<ActiveEffect*>* a_list)
	{
		auto result = hash::fnv1::fnv_offset_basis;

		constexpr auto hashflags =
			ActiveEffect::Flag::kDispelled |
			ActiveEffect::Flag::kInactive;

		visit_effects(
			a_list,
			[&](auto* a_effect, auto* a_mgef) [[msvc::forceinline]] {
				result = hash::fnv1::_append_hash_fnv1a(result, a_mgef->formID);
				result = hash::fnv1::_append_hash_fnv1a(result, a_effect->flags & hashflags);
			});

		return result;
	}

	CachedActorData::CachedActorData(Actor* a_actor) :
		CachedFactionData(a_actor),
		CachedActiveEffectData(a_actor),
		cellAttached(a_actor->IsParentCellAttached()),
		inInterior(a_actor->IsInInteriorCell()),
		worldspace(a_actor->GetParentCellWorldspace()),
		currentPackage(a_actor->GetCurrentPackage()),
		currentIdle(a_actor->GetFurnitureIdle()),
		inCombat(Game::GetActorInCombat(a_actor)),
		flags1(a_actor->flags1 & ACTOR_CHECK_FLAGS_1),
		flags2(a_actor->flags2 & ACTOR_CHECK_FLAGS_2),
		flagslf1(a_actor->flags1 & ACTOR_CHECK_FLAGS_LF_1),
		flagslf2(a_actor->flags2 & ACTOR_CHECK_FLAGS_LF_2),
		swimming(a_actor->IsSwimming()),
		sitting(a_actor->IsSitting()),
		sleeping(a_actor->IsSleeping()),
		beingRidden(a_actor->IsBeingRidden()),
		weaponDrawn(a_actor->IsWeaponDrawn()),
		arrested(a_actor->IsArrested()),
		unconscious(a_actor->IsUnconscious()),
		flying(a_actor->IsFlying())
	{
	}

	template <class Tv>
	inline static constexpr void state_var_update(
		Tv&       a_var,
		const Tv& a_current,
		bool&     a_result) noexcept
	{
		if (a_var != a_current)
		{
			a_var    = a_current;
			a_result = true;
		}
	}

	bool CachedActorData::UpdateState(
		Actor*         a_actor,
		TESObjectCELL* a_cell)
	{
		bool result = false;

		state_var_update(inInterior, a_cell->IsInterior(), result);
		state_var_update(worldspace, a_cell->GetWorldSpace(), result);
		state_var_update(currentIdle, a_actor->GetFurnitureIdle(), result);
		state_var_update(inCombat, Game::GetActorInCombat(a_actor), result);
		state_var_update(flags1.value, a_actor->flags1 & ACTOR_CHECK_FLAGS_1, result);
		state_var_update(flags2.value, a_actor->flags2 & ACTOR_CHECK_FLAGS_2, result);
		state_var_update(sitting, a_actor->IsSitting(), result);
		state_var_update(sleeping, a_actor->IsSleeping(), result);
		state_var_update(beingRidden, a_actor->IsBeingRidden(), result);
		state_var_update(weaponDrawn, a_actor->IsWeaponDrawn(), result);
		state_var_update(arrested, a_actor->IsArrested(), result);
		state_var_update(unconscious, a_actor->IsUnconscious(), result);
		state_var_update(flying, a_actor->IsFlying(), result);

		return result;
	}

	bool CachedActorData::UpdateStateLF(Actor* a_actor)
	{
		bool result = false;

		state_var_update(flagslf1.value, a_actor->flags1 & ACTOR_CHECK_FLAGS_LF_1, result);
		state_var_update(flagslf2.value, a_actor->flags2 & ACTOR_CHECK_FLAGS_LF_2, result);
		state_var_update(swimming, a_actor->IsSwimming(), result);

		return result;
	}

	bool CachedActorData::UpdateStateHF(Actor* a_actor)
	{
		bool result = false;

		state_var_update(currentPackage, a_actor->GetCurrentPackage(), result);

		return result;
	}

}