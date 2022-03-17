#pragma once

#include "FormCommon.h"
#include "FormHolder.h"
#include "WeatherClassificationFlags.h"

#include <ext/Sky.h>
#include <ext/TESWeather.h>

namespace IED
{
	struct CommonParams
	{
		Actor* const                                                 actor;
		TESNPC* const                                                npc;
		TESRace* const                                               race;
		mutable stl::optional<TESFurniture*>                         furniture;
		mutable stl::optional<Game::ObjectRefHandle>                 furnHandle;
		mutable stl::optional<bool>                                  layingDown;
		mutable stl::optional<Biped*>                                biped;
		mutable stl::optional<TESObjectARMO*>                        actorSkin;
		mutable stl::optional<bool>                                  canDualWield;
		mutable stl::optional<bool>                                  isDead;
		mutable stl::optional<bool>                                  inInterior;
		mutable stl::optional<BGSLocation*>                          location;
		mutable stl::optional<TESWorldSpace*>                        worldspace;
		mutable stl::optional<TESCombatStyle*>                       combatStyle;
		mutable stl::optional<RE::TESWeather*>                       currentWeather;
		mutable std::optional<stl::flag<WeatherClassificationFlags>> weatherClass;

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
					if (actor->actorState.actorState1.sitSleepState == ActorState::SIT_SLEEP_STATE::kIsSitting ||
					    actor->actorState.actorState1.sitSleepState == ActorState::SIT_SLEEP_STATE::kIsSleeping)
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
				biped = actor->GetBiped(false).get();
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

		[[nodiscard]] inline constexpr bool test_equipment_flags(TESRace::EquipmentFlag a_mask) const noexcept
		{
			return a_mask && race->validEquipTypes.test(a_mask);
		}
	};
}