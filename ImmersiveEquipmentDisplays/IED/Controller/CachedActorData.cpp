#include "pch.h"

#include "CachedActorData.h"

namespace IED
{

	CachedFactionData::CachedFactionData(Actor* a_actor) :
		active_container(std::addressof(b1))
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

		auto& data = GetWorkingContainer();

		data.clear();

		if (const auto* extraFactionChanges = a_actor->extraData.Get<ExtraFactionChanges>())
		{
			for (const auto& info : extraFactionChanges->factions)
			{
				if (auto faction = info.faction)
				{
					data.emplace(faction, info.rank);
				}
			}
		}

		for (const auto& info : npc->factions)
		{
			if (auto faction = info.faction)
			{
				data.emplace(faction, info.rank);
			}
		}

		const bool result = !BuffersEqual();

		if (result)
		{
			SwapContainers();
		}

		return result;
	}

	CachedActorData::CachedActorData(Actor* a_actor) :
		CachedFactionData(a_actor),
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
		state_var_update(currentPackage, a_actor->GetCurrentPackage(), result);
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

}