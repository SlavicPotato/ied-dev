#pragma once

#include "FormCommon.h"
#include "FormHolder.h"

namespace IED
{
	struct CommonParams
	{
		Actor* actor;
		TESNPC* npc;
		TESRace* race;
		mutable stl::optional<TESFurniture*> furniture;
		mutable stl::optional<Game::ObjectRefHandle> furnHandle;
		mutable stl::optional<bool> layingDown;
		mutable stl::optional<Biped*> biped;
		mutable stl::optional<TESObjectARMO*> actorSkin;
		mutable stl::optional<bool> canDualWield;
		mutable stl::optional<bool> isDead;
		mutable stl::optional<bool> inInterior;
		mutable stl::optional<BGSLocation*> location;
		mutable stl::optional<TESWorldSpace*> worldspace;

		[[nodiscard]] bool get_using_furniture() const
		{
			if (!furnHandle)
			{
				furnHandle = Game::ObjectRefHandle{};

				if (auto pm = actor->processManager)
				{
					if (actor->actorState.actorState1.sitSleepState == ActorState::SIT_SLEEP_STATE::kIsSitting ||
					    actor->actorState.actorState1.sitSleepState == ActorState::SIT_SLEEP_STATE::kIsSleeping)
					{
						furnHandle = pm->GetOccupiedFurniture();
					}
				}
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
				actorSkin = Game::GetActorSkin(actor);
			}

			return *actorSkin;
		}

		[[nodiscard]] constexpr bool can_dual_wield() const
		{
			if (!canDualWield)
			{
				if (actor == *g_thePlayer)
				{
					canDualWield = true;
				}
				else
				{
					if ((race->data.raceFlags & TESRace::kRace_CanDualWield) == TESRace::kRace_CanDualWield)
					{
						if (auto extraCombatStyle = actor->extraData.Get<ExtraCombatStyle>())
						{
							if (auto cs = extraCombatStyle->combatStyle)
							{
								canDualWield = (cs->flags & TESCombatStyle::kFlag_AllowDualWielding) == TESCombatStyle::kFlag_AllowDualWielding;
								return *canDualWield;
							}
						}

						if (auto cs = npc->combatStyle)
						{
							canDualWield = (cs->flags & TESCombatStyle::kFlag_AllowDualWielding) == TESCombatStyle::kFlag_AllowDualWielding;
							return *canDualWield;
						}
					}

					canDualWield = false;
				}
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

		[[nodiscard]] constexpr auto get_location() const
		{
			if (!location)
			{
				if (actor == *g_thePlayer)
				{
					location = (*g_thePlayer)->currentLocation;
				}
				else
				{
					if (auto extraLocation = actor->extraData.Get<ExtraLocation>())
					{
						location = extraLocation->location;
					}
					else
					{
						location = nullptr;
					}
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

		[[nodiscard]] inline constexpr bool test_equipment_flags(TESRace::EquipmentFlag a_mask) const noexcept
		{
			return a_mask && (race->validEquipTypes & a_mask) == a_mask;
		}
	};
}