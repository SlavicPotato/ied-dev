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
		Actor* const                                                 actor;
		TESNPC* const                                                npc;
		TESNPC* const                                                npcOrTemplate;
		TESRace* const                                               race;
		ActorObjectHolder&                                           objects;
		Controller&                                                  controller;
		mutable std::optional<TESFurniture*>                         furniture;
		mutable std::optional<Game::ObjectRefHandle>                 furnHandle;
		mutable std::optional<bool>                                  layingDown;
		mutable std::optional<Biped*>                                biped;
		mutable std::optional<TESObjectARMO*>                        actorSkin;
		mutable std::optional<bool>                                  canDualWield;
		mutable std::optional<bool>                                  isDead;
		mutable std::optional<bool>                                  inInterior;
		mutable std::optional<BGSLocation*>                          location;
		mutable std::optional<TESWorldSpace*>                        worldspace;
		mutable std::optional<TESCombatStyle*>                       combatStyle;
		mutable std::optional<RE::TESWeather*>                       currentWeather;
		mutable std::optional<stl::flag<WeatherClassificationFlags>> weatherClass;
		mutable std::optional<BIPED_OBJECT>                          shieldSlot;
		mutable std::optional<Data::TimeOfDay>                       timeOfDay;
		mutable std::optional<bool>                                  inCombat;

		[[nodiscard]] inline constexpr bool is_player() const noexcept
		{
			return actor == *g_thePlayer;
		}

		[[nodiscard]] bool get_using_furniture() const
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

				furnHandle = handle;
			}

			return *furnHandle != Game::ObjectRefHandle{};
		}

		[[nodiscard]] auto get_furniture() const
		{
			if (!furniture)
			{
				if (get_using_furniture())
				{
					NiPointer<TESObjectREFR> ref;
					if (furnHandle->Lookup(ref))
					{
						if (auto base = ref->baseForm)
						{
							furniture = base->As<TESFurniture>();
							return *furniture;
						}
					}
				}

				furniture = nullptr;
			}

			return *furniture;
		}

		[[nodiscard]] bool get_laying_down() const
		{
			if (!layingDown)
			{
				layingDown = false;

				if (auto furn = get_furniture())
				{
					if (auto kw = FormHolder::GetSingleton().layDown)
					{
						if (IFormCommon::HasKeyword(furn, kw))
						{
							return *(layingDown = true);
						}
					}

					layingDown = furn->furnFlags.test(
						TESFurniture::ActiveMarker::kMustExitToTalk |
						TESFurniture::ActiveMarker::kUnknown31);
				}
			}

			return *layingDown;
		}

		[[nodiscard]] constexpr auto get_biped() const
		{
			if (!biped)
			{
				biped = actor->GetBiped1(false).get();
			}

			return *biped;
		}

		[[nodiscard]] constexpr auto get_actor_skin() const
		{
			if (!actorSkin)
			{
				actorSkin = actor->GetSkin();
			}

			return *actorSkin;
		}

		[[nodiscard]] constexpr auto get_combat_style() const
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

				combatStyle = cs;
			}

			return *combatStyle;
		}

		[[nodiscard]] constexpr bool can_dual_wield() const
		{
			if (!canDualWield)
			{
				bool result = false;

				if (is_player())
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

				canDualWield = result;
			}

			return *canDualWield;
		}

		[[nodiscard]] constexpr bool get_actor_dead() const
		{
			if (!isDead)
			{
				isDead = actor->IsDead();
			}

			return *isDead;
		}

		[[nodiscard]] constexpr bool get_in_interior() const
		{
			if (!inInterior)
			{
				inInterior = actor->IsInInteriorCell();
			}

			return *inInterior;
		}

		[[nodiscard]] constexpr auto get_current_location() const
		{
			if (!location)
			{
				if (actor == *g_thePlayer)
				{
					location = (*g_thePlayer)->currentLocation;
				}
				else
				{
					location = actor->GetCurrentLocation();
				}
			}

			return *location;
		}

		[[nodiscard]] constexpr auto get_worldspace() const
		{
			if (!worldspace)
			{
				worldspace = actor->GetWorldspace();
			}

			return *worldspace;
		}

		[[nodiscard]] inline constexpr auto is_player_teammate() const noexcept
		{
			return actor != *g_thePlayer &&
			       actor->IsPlayerTeammate();
		}

		[[nodiscard]] constexpr auto get_current_weather() const
		{
			if (!currentWeather)
			{
				currentWeather = RE::Sky::GetCurrentWeather();
			}

			return *currentWeather;
		}

		[[nodiscard]] constexpr auto get_weather_class() const
		{
			if (!weatherClass)
			{
				if (auto w = get_current_weather())
				{
					auto f       = w->data.flags & RE::TESWeather::WeatherDataFlag::kWeatherMask;
					weatherClass = static_cast<WeatherClassificationFlags>(f);
				}
				else
				{
					weatherClass = WeatherClassificationFlags::kNone;
				}
			}

			return *weatherClass;
		}

		[[nodiscard]] constexpr auto get_shield_slot() const
		{
			if (!shieldSlot)
			{
				shieldSlot = actor->GetShieldBipedObject();
			}

			return *shieldSlot;
		}

		[[nodiscard]] constexpr auto get_time_of_day() const
		{
			if (!timeOfDay)
			{
				timeOfDay = Data::GetTimeOfDay(RE::Sky::GetSingleton());
			}

			return *timeOfDay;
		}

		[[nodiscard]] constexpr auto is_in_combat() const
		{
			if (!inCombat)
			{
				inCombat = Game::GetActorInCombat(actor);
			}

			return *inCombat;
		}

		[[nodiscard]] inline constexpr bool test_equipment_flags(TESRace::EquipmentFlag a_mask) const noexcept
		{
			return a_mask && race->validEquipTypes.test(a_mask);
		}
	};
}