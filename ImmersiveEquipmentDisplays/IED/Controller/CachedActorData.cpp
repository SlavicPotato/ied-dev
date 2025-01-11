#include "pch.h"

#include "CachedActorData.h"

#include "Common/VectorMath.h"
#include "IED/AreaLightingDetection.h"
#include "IED/CharacterLightingDetection.h"

namespace IED
{
	static constexpr float LD_THROTTLE_RATE = 3.0f;

	CachedPerkData::CachedPerkData(Actor* a_actor) noexcept :
		currentSignature(GetSignature(a_actor))
	{
		const auto npc = a_actor->GetActorBase();
		assert(npc);
		GenerateData(a_actor, npc);
	}

	bool CachedPerkData::UpdatePerks(Actor* a_actor, TESNPC* a_npc) noexcept
	{
		const auto sig = GetSignature(a_actor);

		if (sig != currentSignature)
		{
			currentSignature = sig;

			GenerateData(a_actor, a_npc);
			return true;
		}
		else
		{
			return false;
		}
	}

	void CachedPerkData::GenerateData(Actor* a_actor, TESNPC* a_npc) noexcept
	{
		data.clear();

		if (a_actor->processManager &&
		    a_actor->processManager->middleProcess)
		{
			if (a_actor == *g_thePlayer)
			{
				for (const auto* const e : static_cast<const PlayerCharacter*>(a_actor)->addedPerks)
				{
					if (e)
					{
						if (const auto* const perk = e->perk)
						{
							data.emplace(perk->formID, e->currentRank);
						}
					}
				}
			}

			a_npc->VisitPerks([&](const auto& a_perkRankData) noexcept [[msvc::forceinline]] {
				data.emplace(a_perkRankData.perk->formID, a_perkRankData.currentRank);
			});
		}
	}

	auto CachedPerkData::GetSignature(Actor* a_actor) noexcept
		-> hasher::value_type
	{
		hasher h;

		if (a_actor->processManager &&
		    a_actor->processManager->middleProcess)
		{
			h.append_type<std::uint8_t>(1);

			if (a_actor == *g_thePlayer)
			{
				for (const auto* const e : static_cast<const PlayerCharacter*>(a_actor)->addedPerks)
				{
					if (e)
					{
						if (const auto* const perk = e->perk)
						{
							h.append_type(e->currentRank);
							h.append_type(perk->formID.get());
						}
					}
				}
			}
		}

		return h.finish();
	}

	CachedFactionData::CachedFactionData(Actor* a_actor) noexcept
	{
		const auto* const extraFactionChanges =
			a_actor->extraData.Get<ExtraFactionChanges>();

		const auto npc = a_actor->GetActorBase();
		assert(npc);

		currentSignature = GetSignature(extraFactionChanges, npc);
		GenerateData(extraFactionChanges, npc);
	}

	bool CachedFactionData::UpdateFactions(Actor* a_actor, TESNPC* a_npc) noexcept
	{
		const auto* const extraFactionChanges =
			a_actor->extraData.Get<ExtraFactionChanges>();

		const auto sig = GetSignature(extraFactionChanges, a_npc);

		if (sig != currentSignature)
		{
			currentSignature = sig;
			GenerateData(extraFactionChanges, a_npc);

			return true;
		}
		else
		{
			return false;
		}
	}

	void CachedFactionData::GenerateData(
		const ExtraFactionChanges* a_factionChanges,
		TESNPC*                    a_npc) noexcept
	{
		data.clear();

		visit_factions(
			a_factionChanges,
			a_npc,
			[&](const auto& a_info) noexcept {
				data.emplace(a_info.faction, a_info.rank);
			});
	}

	auto CachedFactionData::GetSignature(
		const ExtraFactionChanges* a_factionChanges,
		TESNPC*                    a_npc) noexcept
		-> hasher::value_type
	{
		hasher h;

		visit_factions(
			a_factionChanges,
			a_npc,
			[&](const auto& a_info) noexcept [[msvc::forceinline]] {
				h.append_type(a_info.rank);
				h.append_type(a_info.faction->formID.get());
			});

		return h.finish();
	}

	CachedActiveEffectData::CachedActiveEffectData(Actor* a_actor) noexcept
	{
		const auto list = a_actor->GetActiveEffectList();

		currentSignature = GetSignature(list);
		GenerateData(list);
	}

	bool CachedActiveEffectData::UpdateEffects(Actor* a_actor) noexcept
	{
		const auto list = a_actor->GetActiveEffectList();

		const auto sig = GetSignature(list);

		if (sig != currentSignature)
		{
			currentSignature = sig;

			GenerateData(list);

			return true;
		}
		else
		{
			return false;
		}
	}

	bool CachedActiveEffectData::HasEffectWithKeyword(
		const BGSKeyword* a_keyword) const noexcept
	{
		if (a_keyword)
		{
			for (const auto* const& e : data)
			{
				if (e->HasKeyword(a_keyword))
				{
					return true;
				}
			}
		}

		return false;
	}

	void CachedActiveEffectData::GenerateData(
		RE::BSSimpleList<ActiveEffect*>* a_list) noexcept
	{
		data.clear();

		visit_effects(
			a_list,
			[&](const auto* a_effect, auto* a_mgef) noexcept [[msvc::forceinline]] {
				if (!a_effect->flags.test_any(
						ActiveEffect::Flag::kDispelled |
						ActiveEffect::Flag::kInactive))
				{
					data.emplace(a_mgef);
				}
			});
	}

	auto CachedActiveEffectData::GetSignature(
		RE::BSSimpleList<ActiveEffect*>* a_list) noexcept
		-> hasher::value_type
	{
		hasher h;

		constexpr auto hashflags =
			ActiveEffect::Flag::kDispelled |
			ActiveEffect::Flag::kInactive;

		visit_effects(
			a_list,
			[&](const auto* a_effect, const auto* a_mgef) noexcept [[msvc::forceinline]] {
				h.append_type(a_effect->flags & hashflags);
				h.append_type(a_mgef->formID.get());
			});

		return h.finish();
	}

	CachedActorData::CachedActorData(Actor* a_actor) noexcept :
		CachedFactionData(a_actor),
		CachedActiveEffectData(a_actor),
		CachedPerkData(a_actor),
		cell(a_actor->GetParentCell()),
		active(a_actor->IsParentCellAttached()),
		inInterior(a_actor->IsInInteriorCell()),
		worldspace(a_actor->GetParentCellWorldspace()),
		currentPackage(a_actor->GetCurrentPackage()),
		currentIdle(a_actor->GetFurnitureIdle()),
		combatStyle(a_actor->GetCombatStyle()),
		inCombat(Game::GetActorInCombat(a_actor)),
		flags1(a_actor->flags1 & ACTOR_CHECK_FLAGS_1),
		flags2(a_actor->flags2 & ACTOR_CHECK_FLAGS_2),
		flagslf1(a_actor->flags1 & ACTOR_CHECK_FLAGS_LF_1),
		flagslf2(a_actor->flags2 & ACTOR_CHECK_FLAGS_LF_2),
		baseFlags(a_actor->GetActorBase()->actorData.actorBaseFlags & NPC_BASE_CHECK_FLAGS),
		lifeState(a_actor->GetLifeState()),
		level(a_actor->GetLevel()),
		swimming(a_actor->IsSwimming()),
		sitting(a_actor->IsSitting()),
		sleeping(a_actor->IsSleeping()),
		beingRidden(a_actor->IsMountRidden()),
		weaponDrawn(a_actor->IsWeaponDrawn()),
		arrested(a_actor->IsArrested()),
		flying(a_actor->IsFlying()),
		sneaking(a_actor->IsSneaking())
	{
		const bool interior = lighting_interior();
		currentDll          = GetLightLevel(a_actor, interior);
		lastDll             = currentDll;
		actorInDarkness          = GetActorInDarkness(a_actor, currentDll, interior);
	}

	template <class Tv>
	static constexpr void state_var_update(
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
		const Actor*   a_actor,
		TESObjectCELL* a_cell) noexcept
	{
		bool result = false;

		state_var_update(cell, a_cell, result);
		state_var_update(inInterior, a_cell->IsInterior(), result);
		state_var_update(worldspace, a_cell->GetWorldSpace(), result);
		state_var_update(currentIdle, a_actor->GetFurnitureIdle(), result);
		state_var_update(inCombat, Game::GetActorInCombat(a_actor), result);
		state_var_update(flags1.value, a_actor->flags1 & ACTOR_CHECK_FLAGS_1, result);
		state_var_update(flags2.value, a_actor->flags2 & ACTOR_CHECK_FLAGS_2, result);
		state_var_update(lifeState, a_actor->GetLifeState(), result);
		state_var_update(sitting, a_actor->IsSitting(), result);
		state_var_update(sleeping, a_actor->IsSleeping(), result);
		state_var_update(sneaking, a_actor->IsSneaking(), result);
		state_var_update(weaponDrawn, a_actor->IsWeaponDrawn(), result);
		state_var_update(flying, a_actor->IsFlying(), result);
		state_var_update(arrested, a_actor->IsArrested(), result);
		state_var_update(beingRidden, a_actor->IsMountRidden(), result);

		if (const auto npc = a_actor->GetActorBase())
		{
			state_var_update(
				baseFlags.value,
				npc->actorData.actorBaseFlags & NPC_BASE_CHECK_FLAGS,
				result);
		}

		tAccum1 = std::min(tAccum1 + *Game::g_frameTimerSlow, LD_THROTTLE_RATE + 1.0f);

		return result;
	}

	bool CachedActorData::UpdateStateLF(
		const Actor* a_actor) noexcept
	{
		bool result = false;

		state_var_update(currentPackage, a_actor->GetCurrentPackage(), result);
		state_var_update(flagslf1.value, a_actor->flags1 & ACTOR_CHECK_FLAGS_LF_1, result);
		state_var_update(flagslf2.value, a_actor->flags2 & ACTOR_CHECK_FLAGS_LF_2, result);
		state_var_update(swimming, a_actor->IsSwimming(), result);
		state_var_update(combatStyle, a_actor->GetCombatStyle(), result);

		return result;
	}

	bool CachedActorData::UpdateStateHF(
		const Actor* a_actor) noexcept
	{
		bool result = false;

		// none for now

		return result;
	}

	bool CachedActorData::DoLFUpdates(
		Actor* a_actor) noexcept
	{
		bool result = false;

		if (const auto npc = a_actor->GetActorBase())
		{
			result |= UpdateFactions(a_actor, npc);
			result |= UpdatePerks(a_actor, npc);
		}

		state_var_update(level, a_actor->GetLevel(), result);

		UpdateActorInDarkness(a_actor, result);

		return result;
	}

	void CachedActorData::UpdateActorInDarkness(
		Actor* a_actor,
		bool&  a_result)
	{
		const bool interior = lighting_interior();
		currentDll += (GetLightLevel(a_actor, interior) - currentDll) * 0.5f;
		if (std::fabsf(currentDll - lastDll) > 0.075f)
		{
			const bool ida = GetActorInDarkness(a_actor, currentDll, interior);
			if ((queuedDarknessVal && *queuedDarknessVal != ida) || actorInDarkness != ida)
			{
				if (!queuedDarknessVal)
				{
					tAccum1 = 0.0f;
				}
				queuedDarknessVal.emplace(ida);
				lastDll = currentDll;
			}
		}

		if (queuedDarknessVal)
		{
			const bool val = *queuedDarknessVal;
			if (val || tAccum1 > LD_THROTTLE_RATE)
			{
				if (val != actorInDarkness)
				{
					actorInDarkness = val;
					a_result   = true;
				}
				queuedDarknessVal.reset();
			}
		}
	}

	bool CachedActorData::GetActorInDarkness(
		Actor* a_actor,
		float  a_lightLevel,
		bool   a_interior) const noexcept
	{
		if (a_interior)
		{
			return a_lightLevel < ALD::GetInteriorAmbientLightThreshold();
		}
		else
		{
			return a_lightLevel < ALD::GetTorchLightLevel(RE::TES::GetSingleton()->sky);
		}
	}

	float CachedActorData::GetLightLevel(
		Actor* a_actor,
		bool   a_interior) const noexcept
	{
		auto sky = RE::TES::GetSingleton()->sky;
		const auto diffuseLightLevel = CLD::GetLightLevel(a_actor);

		if (a_interior)
		{
			const float interiorAmbientLightLevel = ALD::GetInteriorAmbientLightLevel(
				a_actor,
				sky,
				cell);

			return std::max(interiorAmbientLightLevel, 0.0f) + diffuseLightLevel;
		}
		else
		{
			const auto exteriorAmbientLightLevel = sky ? ALD::GetExteriorAmbientLightLevel(sky) : 0.0f;
			return exteriorAmbientLightLevel + diffuseLightLevel;
		}
	}

}