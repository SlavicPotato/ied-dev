#pragma once

#include "FormCommon.h"
#include "FormHolder.h"
#include "TimeOfDay.h"
#include "WeatherClassificationFlags.h"

#include <ext/Sky.h>
#include <ext/TESWeather.h>

namespace IED
{
	class ActorObjectHolder;
	class Controller;

	struct CommonParams
	{
	public:
		inline constexpr CommonParams(
			Actor* const       a_actor,
			TESNPC* const      a_npc,
			TESNPC* const      a_npcOrTemplate,
			TESRace* const     a_race,
			NiNode* const      a_root,
			NiNode* const      a_npcroot,
			ActorObjectHolder& a_holder,
			Controller&        a_controller) noexcept :
			actor(a_actor),
			npc(a_npc),
			npcOrTemplate(a_npcOrTemplate),
			race(a_race),
			root(a_root),
			npcRoot(a_npcroot),
			objects(a_holder),
			controller(a_controller)
		{
		}

		Actor* const       actor;
		TESNPC* const      npc;
		TESNPC* const      npcOrTemplate;
		TESRace* const     race;
		NiNode* const      root;
		NiNode* const      npcRoot;
		ActorObjectHolder& objects;
		Controller&        controller;

		[[nodiscard]] bool is_player() const noexcept;

		[[nodiscard]] Game::ObjectRefHandle get_current_furniture_handle() const;

		[[nodiscard]] bool is_using_furniture() const;

		[[nodiscard]] TESFurniture* get_furniture() const;

		[[nodiscard]] bool get_laying_down() const;

		[[nodiscard]] constexpr auto get_biped() const
		{
			if (!biped)
			{
				biped.emplace(actor->GetBiped1(false).get());
			}

			return *biped;
		}

		[[nodiscard]] constexpr auto get_actor_skin() const
		{
			if (!actorSkin)
			{
				actorSkin.emplace(actor->GetSkin());
			}

			return *actorSkin;
		}

		[[nodiscard]] TESCombatStyle* get_combat_style() const;

		[[nodiscard]] bool can_dual_wield() const;

		[[nodiscard]] constexpr bool get_actor_dead() const
		{
			if (!isDead)
			{
				isDead.emplace(actor->IsDead());
			}

			return *isDead;
		}

		[[nodiscard]] inline constexpr auto get_current_location() const
		{
			if (!location)
			{
				if (is_player())
				{
					location.emplace((*g_thePlayer)->currentLocation);
				}
				else
				{
					location.emplace(actor->GetCurrentLocation());
				}
			}

			return *location;
		}

		[[nodiscard]] inline constexpr bool is_player_teammate() const noexcept
		{
			return actor != *g_thePlayer &&
			       actor->IsPlayerTeammate();
		}

		[[nodiscard]] constexpr auto get_current_weather() const
		{
			if (!currentWeather)
			{
				currentWeather.emplace(RE::Sky::GetCurrentWeather());
			}

			return *currentWeather;
		}

		[[nodiscard]] inline constexpr auto get_weather_class() const
		{
			if (!weatherClass)
			{
				if (const auto w = get_current_weather())
				{
					auto f = w->data.flags & RE::TESWeather::WeatherDataFlag::kWeatherMask;
					weatherClass.emplace(static_cast<WeatherClassificationFlags>(f));
				}
				else
				{
					weatherClass.emplace(WeatherClassificationFlags::kNone);
				}
			}

			return *weatherClass;
		}

		[[nodiscard]] inline constexpr auto get_shield_slot() const
		{
			if (!shieldSlot)
			{
				shieldSlot.emplace(actor->GetShieldBipedObject());
			}

			return *shieldSlot;
		}

		[[nodiscard]] inline constexpr auto get_time_of_day() const
		{
			if (!timeOfDay)
			{
				timeOfDay.emplace(Data::GetTimeOfDay(RE::Sky::GetSingleton()));
			}

			return *timeOfDay;
		}

		[[nodiscard]] inline constexpr auto is_on_mount() const
		{
			if (!isMounted)
			{
				isMounted.emplace(actor->IsOnMount());
			}

			return *isMounted;
		}

		[[nodiscard]] NiPointer<Actor>& get_mounted_actor() const;
		[[nodiscard]] NiPointer<Actor>& get_mounting_actor() const;

		[[nodiscard]] bool is_in_merchant_faction() const;
		[[nodiscard]] bool is_in_player_enemy_faction() const;

		[[nodiscard]] Actor* get_last_ridden_player_horse() const;

		[[nodiscard]] inline constexpr bool test_equipment_flags(TESRace::EquipmentFlag a_mask) const noexcept
		{
			return a_mask && race->validEquipTypes.test(a_mask);
		}

		[[nodiscard]] bool is_horse() const;
		[[nodiscard]] bool is_mounted_actor_horse() const;

	private:
		mutable std::optional<Game::ObjectRefHandle>                 furnHandle;
		mutable std::optional<TESFurniture*>                         furniture;
		mutable std::optional<Biped*>                                biped;
		mutable std::optional<TESObjectARMO*>                        actorSkin;
		mutable std::optional<BGSLocation*>                          location;
		mutable std::optional<TESCombatStyle*>                       combatStyle;
		mutable std::optional<RE::TESWeather*>                       currentWeather;
		mutable std::optional<stl::flag<WeatherClassificationFlags>> weatherClass;
		mutable std::optional<BIPED_OBJECT>                          shieldSlot;
		mutable std::optional<Data::TimeOfDay>                       timeOfDay;
		mutable std::optional<NiPointer<Actor>>                      mountedActor;
		mutable std::optional<NiPointer<Actor>>                      mountedByActor;
		mutable std::optional<NiPointer<Actor>>                      lastRiddenPlayerHorse;
		mutable std::optional<bool>                                  layingDown;
		mutable std::optional<bool>                                  canDualWield;
		mutable std::optional<bool>                                  isDead;
		mutable std::optional<bool>                                  isInMerchantFaction;
		mutable std::optional<bool>                                  isInPlayerEnemyFaction;
		mutable std::optional<bool>                                  isMounted;
		mutable std::optional<bool>                                  isHorse;
		mutable std::optional<bool>                                  isMountHorse;
	};
}