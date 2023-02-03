#pragma once

#include "ConfigData.h"
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
		CommonParams(
			Actor* const       a_actor,
			TESNPC* const      a_npc,
			TESRace* const     a_race,
			BSFadeNode* const  a_root,
			NiNode* const      a_npcroot,
			ActorObjectHolder& a_holder,
			Controller&        a_controller) noexcept;

		Actor* const                actor;
		TESNPC* const               npc;
		TESNPC* const               npcOrTemplate;
		TESRace* const              race;
		const NiPointer<BSFadeNode> root;
		const NiPointer<NiNode>     npcRoot;
		ActorObjectHolder&          objects;
		Controller&                 controller;
		const Data::ConfigSex       configSex;

		[[nodiscard]] constexpr auto get_sex() const noexcept
		{
			return configSex;
		}

		[[nodiscard]] bool is_player() const noexcept;

		[[nodiscard]] Game::ObjectRefHandle get_current_furniture_handle() const noexcept;

		[[nodiscard]] bool is_using_furniture() const noexcept;

		[[nodiscard]] TESFurniture* get_furniture() const noexcept;

		[[nodiscard]] bool get_laying_down() const noexcept;

		[[nodiscard]] constexpr auto get_biped() const noexcept
		{
			if (!biped)
			{
				biped.emplace(actor->GetBiped1(false).get());
			}

			return *biped;
		}

		[[nodiscard]] constexpr auto get_actor_skin() const noexcept
		{
			if (!actorSkin)
			{
				actorSkin.emplace(actor->GetSkin());
			}

			return *actorSkin;
		}

		[[nodiscard]] TESCombatStyle* get_combat_style() const noexcept;

		[[nodiscard]] bool can_dual_wield() const noexcept;

		[[nodiscard]] constexpr bool get_actor_dead() const noexcept
		{
			if (!isDead)
			{
				isDead.emplace(actor->IsDead());
			}

			return *isDead;
		}

		[[nodiscard]] constexpr auto get_current_location() const noexcept
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

		[[nodiscard]] constexpr bool is_player_teammate() const noexcept
		{
			return actor != *g_thePlayer &&
			       actor->IsPlayerTeammate();
		}

		[[nodiscard]] RE::TESWeather*                       get_current_weather() const noexcept;
		[[nodiscard]] stl::flag<WeatherClassificationFlags> get_weather_class() const noexcept;
		[[nodiscard]] Data::TimeOfDay                       get_time_of_day() const noexcept;
		[[nodiscard]] bool                                  is_area_dark() const noexcept;

		[[nodiscard]] constexpr auto get_npc_shield_slot() const noexcept
		{
			return npc->GetShieldBipedObject();
		}

		[[nodiscard]] constexpr auto get_npc_body_slot() const noexcept
		{
			return npc->GetBodyBipedObject();
		}

		[[nodiscard]] constexpr auto get_npc_head_slot() const noexcept
		{
			return npc->GetHeadBipedObject();
		}

		[[nodiscard]] constexpr auto get_npc_hair_slot() const noexcept
		{
			return npc->GetHairBipedObject();
		}

		[[nodiscard]] constexpr auto get_shield_slot() const noexcept
		{
			return race->data.shieldObject;
		}

		[[nodiscard]] constexpr auto get_body_slot() const noexcept
		{
			return race->data.bodyObject;
		}

		[[nodiscard]] constexpr auto get_head_slot() const noexcept
		{
			return race->data.headObject;
		}

		[[nodiscard]] constexpr auto get_hair_slot() const noexcept
		{
			return race->data.hairObject;
		}

		[[nodiscard]] constexpr auto translate_biped_object(
			BIPED_OBJECT a_object) const noexcept
		{
			if (a_object < BIPED_OBJECT::kTotal)
			{
				return a_object;
			}

			switch (a_object)
			{
			case BIPED_OBJECT::kRaceHead:
				return get_head_slot();
			case BIPED_OBJECT::kRaceHair:
				return get_hair_slot();
			case BIPED_OBJECT::kRaceShield:
				return get_shield_slot();
			case BIPED_OBJECT::kRaceBody:
				return get_body_slot();
			default:
				return BIPED_OBJECT::kNone;
			}
		}

		[[nodiscard]] constexpr auto is_on_mount() const noexcept
		{
			if (!isMounted)
			{
				isMounted.emplace(actor->IsOnMount());
			}

			return *isMounted;
		}

		[[nodiscard]] NiPointer<Actor>& get_mounted_actor() const noexcept;
		[[nodiscard]] NiPointer<Actor>& get_mounting_actor() const noexcept;

		[[nodiscard]] bool is_in_merchant_faction() const noexcept;
		[[nodiscard]] bool is_in_player_enemy_faction() const noexcept;

		[[nodiscard]] NiPointer<Actor>& get_last_ridden_player_horse() const noexcept;

		[[nodiscard]] constexpr bool test_equipment_flags(TESRace::EquipmentFlag a_mask) const noexcept
		{
			return a_mask && race->validEquipTypes.test(a_mask);
		}

		[[nodiscard]] bool is_horse() const noexcept;
		[[nodiscard]] bool is_mounted_actor_horse() const noexcept;

		[[nodiscard]] BGSVoiceType* get_voice_type() const noexcept;

		[[nodiscard]] TESForm* get_parent_cell_owner() const noexcept;

	private:
		mutable std::optional<Game::ObjectRefHandle> furnHandle;
		mutable std::optional<TESFurniture*>         furniture;
		mutable std::optional<Biped*>                biped;
		mutable std::optional<TESObjectARMO*>        actorSkin;
		mutable std::optional<BGSLocation*>          location;
		mutable std::optional<TESCombatStyle*>       combatStyle;
		mutable std::optional<NiPointer<Actor>>      mountedActor;
		mutable std::optional<NiPointer<Actor>>      mountedByActor;
		mutable std::optional<NiPointer<Actor>>      lastRiddenPlayerHorse;
		mutable std::optional<bool>                  layingDown;
		mutable std::optional<bool>                  canDualWield;
		mutable std::optional<bool>                  isDead;
		mutable std::optional<bool>                  isInMerchantFaction;
		mutable std::optional<bool>                  isInPlayerEnemyFaction;
		mutable std::optional<bool>                  isMounted;
		mutable std::optional<bool>                  isHorse;
		mutable std::optional<bool>                  isMountHorse;
		mutable std::optional<TESForm*>              parentCellOwner;
	};
}