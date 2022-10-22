#include "pch.h"

#include "CommonParams.h"

#include "Controller/ActorObjectHolder.h"

namespace IED
{
	template <class Tf>
	inline static constexpr bool match_faction(
		const CachedFactionData& a_data,
		Tf                       a_match) noexcept
	{
		auto& data = a_data.GetFactionContainer();

		auto it = std::find_if(
			data.begin(),
			data.end(),
			a_match);

		return it != data.end();
	}

	bool CommonParams::is_player() const noexcept
	{
		return objects.IsPlayer();
	}

	Game::ObjectRefHandle CommonParams::get_current_furniture_handle() const
	{
		if (!furnHandle)
		{
			Game::ObjectRefHandle handle;

			if (auto pm = actor->processManager)
			{
				if (actor->actorState1.sitSleepState == ActorState::SIT_SLEEP_STATE::kIsSitting ||
				    actor->actorState1.sitSleepState == ActorState::SIT_SLEEP_STATE::kIsSleeping)
				{
					handle = pm->GetOccupiedFurniture();
				}
			}

			furnHandle.emplace(handle);
		}

		return *furnHandle;
	}

	bool CommonParams::is_using_furniture() const
	{
		const auto handle = get_current_furniture_handle();
		return handle && handle.IsValid();
	}

	TESFurniture* CommonParams::get_furniture() const
	{
		if (!furniture)
		{
			if (const auto handle = get_current_furniture_handle();
			    handle && handle.IsValid())
			{
				NiPointer<TESObjectREFR> ref;

				if (handle.Lookup(ref))
				{
					if (auto base = ref->baseForm)
					{
						furniture.emplace(base->As<TESFurniture>());
						return *furniture;
					}
				}
			}

			furniture.emplace(nullptr);
		}

		return *furniture;
	}

	bool CommonParams::get_laying_down() const
	{
		if (!layingDown)
		{
			if (auto furn = get_furniture())
			{
				if (auto kw = FormHolder::GetSingleton().layDown)
				{
					if (IFormCommon::HasKeyword(furn, kw))
					{
						layingDown.emplace(true);
						return *layingDown;
					}
				}

				layingDown.emplace(furn->furnFlags.test(
					TESFurniture::ActiveMarker::kMustExitToTalk |
					TESFurniture::ActiveMarker::kUnknown31));
			}
			else
			{
				layingDown.emplace(false);
			}
		}

		return *layingDown;
	}

	TESCombatStyle* CommonParams::get_combat_style() const
	{
		if (!combatStyle)
		{
			TESCombatStyle* cs = nullptr;

			if (auto extraCombatStyle = actor->extraData.Get<ExtraCombatStyle>())
			{
				cs = extraCombatStyle->combatStyle;
			}

			if (!cs)
			{
				cs = npc->combatStyle;
			}

			combatStyle.emplace(cs);
		}

		return *combatStyle;
	}

	bool CommonParams::can_dual_wield() const
	{
		if (!canDualWield)
		{
			bool result = false;

			if (objects.IsPlayer())
			{
				result = true;
			}
			else
			{
				if (race->data.raceFlags.test(TESRace::Flag::kCanDualWield))
				{
					if (auto cs = get_combat_style())
					{
						result = cs->AllowDualWielding();
					}
				}
			}

			canDualWield.emplace(result);
		}

		return *canDualWield;
	}

	NiPointer<Actor>& CommonParams::get_mounted_actor() const
	{
		if (!mountedActor)
		{
			NiPointer<Actor> tmp;
			if (actor->GetMountedActor(tmp))
			{
				mountedActor.emplace(std::move(tmp));
			}
			else
			{
				mountedActor.emplace(nullptr);
			}
		}

		return *mountedActor;
	}

	NiPointer<Actor>& CommonParams::get_mounting_actor() const
	{
		if (!mountedByActor)
		{
			NiPointer<Actor> tmp;
			if (actor->GetMountedByActor(tmp))
			{
				mountedByActor.emplace(std::move(tmp));
			}
			else
			{
				mountedByActor.emplace(nullptr);
			}
		}

		return *mountedByActor;
	}

	bool CommonParams::is_in_merchant_faction() const
	{
		if (!isInMerchantFaction)
		{
			isInMerchantFaction.emplace(match_faction(
				objects.GetCachedData(),
				[&](auto& a_v) {
					return a_v.first->IsLikelyMerchantFaction();
				}));
		}

		return *isInMerchantFaction;
	}

	bool CommonParams::is_in_player_enemy_faction() const
	{
		if (!isInPlayerEnemyFaction)
		{
			isInPlayerEnemyFaction.emplace(match_faction(
				objects.GetCachedData(),
				[&](auto& a_v) {
					return a_v.first->data.flags.test_any(FACTION_DATA::Flag::kPlayerIsEnemy);
				}));
		}

		return *isInPlayerEnemyFaction;
	}

	Actor* CommonParams::get_last_ridden_player_horse() const
	{
		if (!lastRiddenPlayerHorse)
		{
			lastRiddenPlayerHorse.emplace(nullptr);

			const auto handle = (*g_thePlayer)->lastRiddenHorseHandle;

			if (handle && handle.IsValid())
			{
				(void)handle.Lookup(*lastRiddenPlayerHorse);
			}
		}

		return *lastRiddenPlayerHorse;
	}

	bool CommonParams::is_horse() const
	{
		if (!isHorse)
		{
			isHorse.emplace(actor->IsHorse());
		}

		return *isHorse;
	}

	bool CommonParams::is_mounted_actor_horse() const
	{
		if (!isMountHorse)
		{
			if (auto& e = get_mounted_actor())
			{
				isMountHorse.emplace(e->IsHorse());
			}
			else
			{
				isMountHorse.emplace(false);
			}
		}

		return *isMountHorse;
	}
}
