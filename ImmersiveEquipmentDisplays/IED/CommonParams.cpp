#include "pch.h"

#include "CommonParams.h"

#include "Controller/ActorObjectHolder.h"

#include "AreaLightingDetection.h"

#include "IED/Controller/Controller.h"

namespace IED
{
	template <class Tf>
	static constexpr bool match_faction(
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

	CommonParams::CommonParams(
		Actor* const       a_actor,
		TESNPC* const      a_npc,
		TESRace* const     a_race,
		BSFadeNode* const  a_root,
		NiNode* const      a_npcroot,
		ActorObjectHolder& a_holder,
		Controller&        a_controller) noexcept :
		actor(a_actor),
		npc(a_npc),
		npcOrTemplate(a_npc->GetFirstNonTemporaryOrThis()),
		race(a_race),
		root(a_root),
		npcRoot(a_npcroot),
		objects(a_holder),
		controller(a_controller),
		configSex(a_holder.GetSex())
	{
	}

	bool CommonParams::is_player() const noexcept
	{
		return objects.IsPlayer();
	}

	Game::ObjectRefHandle CommonParams::get_current_furniture_handle() const noexcept
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
					if (!handle.IsValid())
					{
						handle = Game::ObjectRefHandle{};
					}
				}
			}

			furnHandle.emplace(handle);
		}

		return *furnHandle;
	}

	bool CommonParams::is_using_furniture() const noexcept
	{
		const auto handle = get_current_furniture_handle();
		return static_cast<bool>(handle);
	}

	TESFurniture* CommonParams::get_furniture() const noexcept
	{
		if (!furniture)
		{
			if (const auto handle = get_current_furniture_handle();
			    static_cast<bool>(handle))
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

	bool CommonParams::get_laying_down() const noexcept
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

	bool CommonParams::can_dual_wield() const noexcept
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
					if (const auto* const style = objects.GetCachedData().combatStyle)
					{
						result = style->AllowDualWielding();
					}
				}
			}

			canDualWield.emplace(result);
		}

		return *canDualWield;
	}

	RE::TESWeather* CommonParams::get_current_weather() const noexcept
	{
		return controller.GetOrCreateGlobalParams().get_current_weather();
	}

	stl::flag<WeatherClassificationFlags> CommonParams::get_weather_class() const noexcept
	{
		if (const auto w = get_current_weather())
		{
			const auto f = w->data.flags & RE::TESWeather::WeatherDataFlag::kWeatherMask;
			return static_cast<WeatherClassificationFlags>(f);
		}
		else
		{
			return WeatherClassificationFlags::kNone;
		}
	}

	Data::TimeOfDay CommonParams::get_time_of_day() const noexcept
	{
		return controller.GetOrCreateGlobalParams().get_time_of_day();
	}

	bool CommonParams::is_in_dark_area() const noexcept
	{
		if (!isInDarkArea)
		{
			const bool result = actor->IsInInterior() ?
			                        ALD::IsActorInDarkInterior(actor, RE::TES::GetSingleton()->sky) :
			                        controller.GetOrCreateGlobalParams().is_exterior_dark();

			isInDarkArea.emplace(result);
		}

		return *isInDarkArea;
	}

	bool CommonParams::is_daytime() const noexcept
	{
		return controller.GetOrCreateGlobalParams().is_daytime();
	}

	float CommonParams::get_sun_angle() const noexcept
	{
		return controller.GetOrCreateGlobalParams().get_sun_angle();
	}

	NiPointer<Actor>& CommonParams::get_mounted_actor() const noexcept
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
				mountedActor.emplace();
			}
		}

		return *mountedActor;
	}

	NiPointer<Actor>& CommonParams::get_mounting_actor() const noexcept
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
				mountedByActor.emplace();
			}
		}

		return *mountedByActor;
	}

	bool CommonParams::is_in_merchant_faction() const noexcept
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

	bool CommonParams::is_in_player_enemy_faction() const noexcept
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

	NiPointer<Actor>& CommonParams::get_last_ridden_player_horse() const noexcept
	{
		if (!lastRiddenPlayerHorse)
		{
			lastRiddenPlayerHorse.emplace();

			const auto handle = (*g_thePlayer)->lastRiddenHorseHandle;

			if (handle && handle.IsValid())
			{
				(void)handle.Lookup(*lastRiddenPlayerHorse);
			}
		}

		return *lastRiddenPlayerHorse;
	}

	bool CommonParams::is_horse() const noexcept
	{
		if (!isHorse)
		{
			isHorse.emplace(actor->IsHorse());
		}

		return *isHorse;
	}

	bool CommonParams::is_mounted_actor_horse() const noexcept
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

	BGSVoiceType* CommonParams::get_voice_type() const noexcept
	{
		auto result = npc->voiceType;

		if (!result)
		{
			result = race->voiceTypes[objects.IsFemale()];
		}

		return result;
	}

	TESForm* CommonParams::get_parent_cell_owner() const noexcept
	{
		if (!parentCellOwner)
		{
			const auto parentCell = actor->GetParentCell();
			parentCellOwner.emplace(parentCell ? parentCell->GetOwnerForm() : nullptr);
		}

		return *parentCellOwner;
	}
}
