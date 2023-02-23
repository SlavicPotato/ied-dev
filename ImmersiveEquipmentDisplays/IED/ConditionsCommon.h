#pragma once

#include "CommonParams.h"
#include "ConditionalVariableStorage.h"
#include "ConfigData.h"
#include "Data.h"
#include "FormCommon.h"
#include "WeatherClassificationFlags.h"

#include "Controller/CachedActorData.h"
#include "Controller/ObjectManagerData.h"

namespace IED
{
	class SkeletonID;

	namespace Conditions
	{

		const ActorObjectMap& get_actor_object_map(CommonParams& a_params) noexcept;

		bool is_in_first_person(CommonParams& a_params) noexcept;
		bool is_sds_shield_on_back_enabled(CommonParams& a_params) noexcept;

		constexpr bool is_player_last_ridden_mount(CommonParams& a_params) noexcept
		{
			return a_params.objects.GetHandle() == (*g_thePlayer)->lastRiddenHorseHandle;
		}

		inline bool match_random_percent(
			CommonParams&   a_params,
			const luid_tag& a_luid,
			float           a_percent) noexcept
		{
			return a_percent >= 100.0f || a_params.objects.GetRandomPercent(a_luid) < a_percent;
		}

		bool                 is_in_dialogue(CommonParams& a_params) noexcept;
		bool                 is_cell_owner(CommonParams& a_params, const CachedFactionData& a_cachedFactionData) noexcept;
		BGSLightingTemplate* get_active_lighting_template(CommonParams& a_params) noexcept;

		template <class Tv, class Tm>
		constexpr bool compare(
			Data::ComparisonOperator a_oper,
			const Tv&                a_value,
			const Tm&                a_match) noexcept
		{
			switch (a_oper)
			{
			case Data::ComparisonOperator::kEqual:
				return a_value == a_match;
			case Data::ComparisonOperator::kNotEqual:
				return a_value != a_match;
			case Data::ComparisonOperator::kGreater:
				return a_value > a_match;
			case Data::ComparisonOperator::kLower:
				return a_value < a_match;
			case Data::ComparisonOperator::kGreaterOrEqual:
				return a_value >= a_match;
			case Data::ComparisonOperator::kLowerOrEqual:
				return a_value <= a_match;
			default:
				return false;
			}
		}

		template <class Tm, class Tf>
		constexpr bool match_extra(
			CommonParams&          a_params,
			const Tm&              a_match,
			const CachedActorData& a_cached) noexcept
		{
			switch (a_match.extraCondType)
			{
			case Data::ExtraConditionType::kCanDualWield:
				return a_params.can_dual_wield();
			case Data::ExtraConditionType::kIsDead:
				return a_params.get_actor_dead();
			case Data::ExtraConditionType::kInInterior:
				return a_cached.inInterior;
			case Data::ExtraConditionType::kIsPlayerTeammate:
				return !a_params.objects.IsPlayer() && a_cached.flags1.test(Actor::Flags1::kPlayerTeammate);
			case Data::ExtraConditionType::kIsGuard:
				return a_cached.flags1.test(Actor::Flags1::kGuard);
			case Data::ExtraConditionType::kIsMount:
				return a_params.actor->IsMount();
			case Data::ExtraConditionType::kShoutEquipped:
				return match_form_with_id<Tm, Tf>(a_match, a_params.actor->selectedPower);
			case Data::ExtraConditionType::kInMerchantFaction:
				return a_params.is_in_merchant_faction();
			case Data::ExtraConditionType::kCombatStyle:
				return match_form_with_id<Tm, Tf>(a_match, a_cached.combatStyle);
			case Data::ExtraConditionType::kClass:
				return match_form_with_id<Tm, Tf>(a_match, a_params.npc->GetClass());
			case Data::ExtraConditionType::kTimeOfDay:
				return match_time_of_day<Tm>(a_params, a_match);
			case Data::ExtraConditionType::kIsInFirstPerson:
				return is_in_first_person(a_params);
			case Data::ExtraConditionType::kInCombat:
				return a_cached.inCombat;
			case Data::ExtraConditionType::kIsFemale:
				return a_params.objects.IsFemale();
#if defined(IED_ENABLE_CONDITION_EN)
			case Data::ExtraConditionType::kPlayerEnemiesNearby:
				return enemies_nearby(a_params);
#endif
			case Data::ExtraConditionType::kInWater:
				return a_cached.flagslf1.test(Actor::Flags1::kInWater);
			case Data::ExtraConditionType::kUnderwater:
				return a_cached.flagslf2.test(Actor::Flags2::kUnderwater);
			case Data::ExtraConditionType::kSwimming:
				return a_cached.swimming;
			case Data::ExtraConditionType::kBleedingOut:
				return a_cached.flags2.test(Actor::Flags2::kInBleedoutAnimation);
			case Data::ExtraConditionType::kTresspassing:
				return a_cached.flags2.test(Actor::Flags2::kIsTrespassing);
			case Data::ExtraConditionType::kIsCommanded:
				return a_cached.flags2.test(Actor::Flags2::kIsCommandedActor);
			case Data::ExtraConditionType::kParalyzed:
				return a_cached.flags1.test(Actor::Flags1::kParalyzed);
			case Data::ExtraConditionType::kIsRidingMount:
				return a_params.is_on_mount();
			case Data::ExtraConditionType::kHumanoidSkeleton:
				return a_params.objects.HasHumanoidSkeleton();
			case Data::ExtraConditionType::kIsPlayer:
				return a_params.objects.IsPlayer();
			case Data::ExtraConditionType::kBribedByPlayer:
				return !a_params.objects.IsPlayer() && a_cached.flags2.test(Actor::Flags2::kBribedByPlayer);
			case Data::ExtraConditionType::kAngryWithPlayer:
				return !a_params.objects.IsPlayer() && a_cached.flags2.test(Actor::Flags2::kAngryWithPlayer);
			case Data::ExtraConditionType::kEssential:
				return a_cached.flags2.test(Actor::Flags2::kEssential);
			case Data::ExtraConditionType::kProtected:
				return a_cached.flags2.test(Actor::Flags2::kProtected);
			case Data::ExtraConditionType::kSitting:
				return a_cached.sitting;
			case Data::ExtraConditionType::kSleeping:
				return a_cached.sleeping;
			case Data::ExtraConditionType::kIsMountRidden:
				return a_cached.beingRidden;
			case Data::ExtraConditionType::kWeaponDrawn:
				return a_cached.weaponDrawn;
			case Data::ExtraConditionType::kRandomPercent:
				return match_random_percent(a_params, a_match, a_match.percent);
			case Data::ExtraConditionType::kNodeMonitor:
				return a_params.objects.GetNodeMonitorResult(a_match.uid);
			case Data::ExtraConditionType::kArrested:
				return a_cached.arrested;
			case Data::ExtraConditionType::kIsChild:
				return a_params.actor->IsChild();
			case Data::ExtraConditionType::kInKillmove:
				return a_cached.flags2.test(Actor::Flags2::kIsInKillMove);
			case Data::ExtraConditionType::kIsUnconscious:
				return a_cached.lifeState == ActorState::ACTOR_LIFE_STATE::kUnconcious;
			case Data::ExtraConditionType::kIsPlayerLastRiddenMount:
				return is_player_last_ridden_mount(a_params);
			case Data::ExtraConditionType::kSDSShieldOnBackEnabled:
				return is_sds_shield_on_back_enabled(a_params);
			case Data::ExtraConditionType::kIsFlying:
				return a_cached.flying;
			case Data::ExtraConditionType::kIsLayingDown:
				return a_params.get_laying_down();
			case Data::ExtraConditionType::kInPlayerEnemyFaction:
				return a_params.is_in_player_enemy_faction();
			case Data::ExtraConditionType::kIsHorse:
				return a_params.is_horse();
			case Data::ExtraConditionType::kIsUnique:
				return a_cached.baseFlags.test(ACTOR_BASE_DATA::Flag::kUnique);
			case Data::ExtraConditionType::kIsSummonable:
				return a_cached.baseFlags.test(ACTOR_BASE_DATA::Flag::kSummonable);
			case Data::ExtraConditionType::kIsInvulnerable:
				return a_cached.baseFlags.test(ACTOR_BASE_DATA::Flag::kInvulnerable);
			case Data::ExtraConditionType::kLevel:
				return compare(a_match.compOperator2, a_cached.level, a_match.level);
			case Data::ExtraConditionType::kDayOfWeek:
				return RE::Calendar::GetSingleton()->GetDayOfWeek() == a_match.dayOfWeek;
			case Data::ExtraConditionType::kIsSneaking:
				return a_cached.sneaking;
			case Data::ExtraConditionType::kInDialogue:
				return is_in_dialogue(a_params);
			case Data::ExtraConditionType::kLifeState:
				return a_cached.lifeState == a_match.lifeState;
			case Data::ExtraConditionType::kActorValue:
				return compare(
					a_match.compOperator2,
					a_params.actor->GetActorValue(a_match.actorValue),
					a_match.avMatch);
			case Data::ExtraConditionType::kXP32Skeleton:
				return a_params.objects.HasXP32Skeleton();
			case Data::ExtraConditionType::kInDarkArea:
				return a_params.is_in_dark_area();
			case Data::ExtraConditionType::kInPublicCell:
				return a_params.actor->InPublicCell();
			case Data::ExtraConditionType::kIsCellOwner:
				return is_cell_owner(a_params, a_cached);
			case Data::ExtraConditionType::kInOwnedCell:
				return static_cast<bool>(a_params.get_parent_cell_owner());
			case Data::ExtraConditionType::kIsNPCCellOwner:
				return a_params.get_parent_cell_owner() == a_params.npc;
			case Data::ExtraConditionType::kSunAngle:
				{
					const auto angle = a_params.get_sun_angle();

					return compare(
						a_match.compOperator2,
						a_match.flags.test(Tf::kExtraFlag1) ? std::fabsf(angle) : angle,
						a_match.sunAngle);
				}
			case Data::ExtraConditionType::kIsSunAboveHorizon:
				return a_params.is_daytime();
			case Data::ExtraConditionType::kKeyIDToggled:
				return a_params.get_key_toggle_state(a_match.s0);
			case Data::ExtraConditionType::kLightingTemplate:
				return match_form_with_id<Tm, Tf>(a_match, get_active_lighting_template(a_params));
			default:
				return false;
			}
		}

		constexpr bool match_form(
			Game::FormID a_formid,
			TESForm*     a_form) noexcept
		{
			return a_formid && a_form->formID == a_formid;
		}

		constexpr bool is_hand_slot(Data::ObjectSlotExtra a_slot) noexcept
		{
			return a_slot != Data::ObjectSlotExtra::kArmor &&
			       a_slot != Data::ObjectSlotExtra::kAmmo;
		}

		constexpr bool is_valid_form_for_slot(
			TESForm*              a_form,
			Data::ObjectSlotExtra a_slot,
			bool                  a_left) noexcept
		{
			return a_left ?
			           Data::ItemData::GetItemSlotLeftExtra(a_form) == a_slot :
			           Data::ItemData::GetItemSlotExtra(a_form) == a_slot;
		}

		constexpr bool is_ammo_bolt(const TESForm* a_form) noexcept
		{
			if (const auto ammo = a_form->As<TESAmmo>())
			{
				return ammo->isBolt();
			}
			else
			{
				return false;
			}
		}

		constexpr bool is_object_visible(const NiPointer<NiAVObject>& a_object) noexcept
		{
			return a_object && a_object->IsVisible();
		}

		bool is_in_location(
			BGSLocation* a_current,
			BGSLocation* a_loc) noexcept;

		bool is_in_location(
			BGSLocation* a_current,
			BGSKeyword*  a_keyword,
			BGSLocation* a_matched) noexcept;

		bool is_in_location(
			BGSLocation* a_current,
			BGSKeyword*  a_keyword) noexcept;

		bool match_worldspace(
			TESWorldSpace* a_current,
			Game::FormID   a_wsId,
			bool           a_matchParent) noexcept;

		template <class Tm, class Tf, class Tp>
		constexpr bool match_biped(
			CommonParams& a_params,
			const Tm&     a_match,
			Tp            a_post)  //
			noexcept    //
			requires(std::invocable<Tp, const TESForm*>)
		{
			const auto slot = a_params.translate_biped_object(a_match.bipedSlot);

			if (slot >= BIPED_OBJECT::kTotal)
			{
				return false;
			}

			const auto* const biped = a_params.get_biped();
			if (!biped)
			{
				return false;
			}

			auto& e = biped->get_object(slot);

			const auto* const form = e.item;
			if (!form || e.addon == form)
			{
				return false;
			}

			if (a_match.flags.test(Tf::kExtraFlag2))
			{
				if (const auto* const skin = a_params.get_actor_skin())
				{
					if (a_match.flags.test(Tf::kNegateMatch1) ==
					    (form == skin))
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				if (a_match.form.get_id())
				{
					if (a_match.flags.test(Tf::kNegateMatch1) ==
					    (form->formID == a_match.form.get_id()))
					{
						return false;
					}
				}
			}

			if (a_match.keyword.get_id())
			{
				if (a_match.flags.test(Tf::kNegateMatch2) ==
				    IFormCommon::HasKeyword(form, a_match.keyword))
				{
					return false;
				}
			}

			if (a_match.flags.test(Tf::kExtraFlag1))
			{
				if (a_match.flags.test(Tf::kNegateMatch3) ==
				    is_ammo_bolt(form))
				{
					return false;
				}
			}

			if (a_match.flags.test(Tf::kExtraFlag3))
			{
				if (a_match.flags.test(Tf::kNegateMatch4) ==
				    is_object_visible(e.object))
				{
					return false;
				}
			}

			a_post(form);

			return true;
		}

		template <class Tm, class Tf>
		constexpr bool match_actor(
			CommonParams& a_params,
			const Tm&     a_match) noexcept
		{
			if (a_match.form.get_id())
			{
				if (a_match.flags.test(Tf::kNegateMatch2) ==
				    (a_params.actor->formID == a_match.form.get_id()))
				{
					return false;
				}
			}

			if (a_match.keyword.get_id())
			{
				if (a_match.flags.test(Tf::kNegateMatch1) ==
				    IFormCommon::HasKeyword(a_params.actor, a_match.keyword))
				{
					return false;
				}
			}

			return true;
		}

		template <class Tm, class Tf>
		constexpr bool match_npc(
			CommonParams& a_params,
			const Tm&     a_match) noexcept
		{
			if (a_match.form.get_id())
			{
				const auto npc =
					a_match.flags.test(Tf::kExtraFlag1) ?
						a_params.npcOrTemplate :
						a_params.npc;

				if (a_match.flags.test(Tf::kNegateMatch2) ==
				    (npc->formID == a_match.form.get_id()))
				{
					return false;
				}
			}

			if (a_match.keyword.get_id())
			{
				if (a_match.flags.test(Tf::kNegateMatch1) ==
				    IFormCommon::HasKeyword(a_params.npc, a_match.keyword))
				{
					return false;
				}
			}

			return true;
		}

		template <class Tm, class Tf>
		constexpr bool match_race(
			CommonParams& a_params,
			const Tm&     a_match) noexcept
		{
			if (a_match.form.get_id())
			{
				if (a_match.flags.test(Tf::kNegateMatch2) ==
				    (a_params.race->formID == a_match.form.get_id()))
				{
					return false;
				}
			}

			if (a_match.keyword.get_id())
			{
				if (a_match.flags.test(Tf::kNegateMatch1) ==
				    IFormCommon::HasKeyword(a_params.race, a_match.keyword))
				{
					return false;
				}
			}

			if (a_match.flags.test(Tf::kExtraFlag1))
			{
				if (a_match.flags.test(Tf::kNegateMatch3) ==
				    a_params.race->data.raceFlags.test(TESRace::Flag::kPlayable))
				{
					return false;
				}
			}

			if (a_match.flags.test(Tf::kExtraFlag2))
			{
				if (a_match.flags.test(Tf::kNegateMatch4) ==
				    a_params.race->data.raceFlags.test(TESRace::Flag::kChild))
				{
					return false;
				}
			}

			return true;
		}

		template <class Tm, class Tf>
		constexpr bool match_furniture(
			CommonParams& a_params,
			const Tm&     a_match) noexcept
		{
			if (a_match.form.get_id())
			{
				auto furn = a_params.get_furniture();
				if (!furn)
				{
					return false;
				}

				if (a_match.flags.test(Tf::kNegateMatch1) ==
				    (furn->formID == a_match.form.get_id()))
				{
					return false;
				}
			}

			if (a_match.keyword.get_id())
			{
				auto furn = a_params.get_furniture();
				if (!furn)
				{
					return false;
				}

				if (a_match.flags.test(Tf::kNegateMatch2) ==
				    IFormCommon::HasKeyword(furn, a_match.keyword))
				{
					return false;
				}
			}

			return a_match.flags.test(Tf::kExtraFlag1) ?
			           a_params.get_laying_down() :
			           a_params.is_using_furniture();
		}

		template <class Tm, class Tf>
		constexpr bool match_location(
			CommonParams& a_params,
			const Tm&     a_match) noexcept
		{
			if (const auto current = a_params.get_current_location())
			{
				if (a_match.flags.test(Tf::kExtraFlag1))
				{
					BGSLocation* location = nullptr;

					if (a_match.form.get_id())
					{
						location = a_match.form.get_form<BGSLocation>();
						if (!location)
						{
							return false;
						}

						if (a_match.flags.test(Tf::kNegateMatch1) ==
						    is_in_location(current, location))
						{
							return false;
						}

						if (a_match.flags.test(Tf::kMatchCategoryOperOR))
						{
							return true;
						}
					}

					if (a_match.keyword.get_id())
					{
						auto keyword = a_match.keyword.get_form<BGSKeyword>();
						if (!keyword)
						{
							return false;
						}

						if (a_match.flags.test(Tf::kNegateMatch2) ==
						    (location ?
						         is_in_location(current, keyword, location) :
						         is_in_location(current, keyword)))
						{
							return false;
						}
					}
				}
				else
				{
					if (a_match.form.get_id())
					{
						if (a_match.flags.test(Tf::kNegateMatch1) ==
						    (a_match.form.get_id() == current->formID))
						{
							return false;
						}

						if (a_match.flags.test(Tf::kMatchCategoryOperOR))
						{
							return true;
						}
					}

					if (a_match.keyword.get_id())
					{
						if (a_match.flags.test(Tf::kNegateMatch2) ==
						    IFormCommon::HasKeyword(current, a_match.keyword))
						{
							return false;
						}
					}
				}

				/*if (a_match.flags.test(Tf::kExtraFlag2))
				{
					if (a_match.flags.test(Tf::kNegateMatch3) ==
					    current->IsCleared())
					{
						return false;
					}
				}*/

				return true;
			}
			else
			{
				return false;
			}
		}

		template <class Tm, class Tf>
		constexpr bool match_worldspace(
			CommonParams&          a_params,
			const Tm&              a_match,
			const CachedActorData& a_cached) noexcept
		{
			if (auto current = a_cached.worldspace)
			{
				if (a_match.form.get_id())
				{
					if (a_match.flags.test(Tf::kNegateMatch1) ==
					    match_worldspace(
							current,
							a_match.form.get_id(),
							a_match.flags.test(Tf::kExtraFlag1)))
					{
						return false;
					}
				}

				return true;
			}
			else
			{
				return false;
			}
		}

		template <class Tm, class Tf>
		constexpr bool match_package(
			CommonParams&          a_params,
			const Tm&              a_match,
			const CachedActorData& a_cached) noexcept
		{
			if (auto current = a_cached.currentPackage)
			{
				if (a_match.form.get_id())
				{
					if (a_match.flags.test(Tf::kNegateMatch1) ==
					    (a_match.form.get_id() == current->formID))
					{
						return false;
					}
				}

				if (a_match.procedureType != PACKAGE_PROCEDURE_TYPE::kNone)
				{
					if (a_match.flags.test(Tf::kNegateMatch2) ==
					    (a_match.procedureType == current->packData.type()))
					{
						return false;
					}
				}

				return true;
			}
			else
			{
				return false;
			}
		}

		template <class Tm, class Tf>
		constexpr bool match_form_with_id(
			const Tm& a_match,
			TESForm*  a_form) noexcept
		{
			if (a_form)
			{
				if (a_match.form.get_id())
				{
					if (a_match.flags.test(Tf::kNegateMatch1) ==
					    (a_match.form.get_id() == a_form->formID))
					{
						return false;
					}
				}

				return true;
			}
			else
			{
				return false;
			}
		}

		/*template <class Tm, class Tf>
		bool match_form_or_template(
			const Tm& a_match,
			TESForm*  a_form)
		{
			if (a_match.form.get_id() == a_form->formID)
			{
				return true;
			}

			if (a_match.flags.test(Tf::kMatchTemplate))
			{
				if (auto armor = a_form->As<TESObjectARMO>())
				{
					if (auto tnam = armor->templateArmor)
					{
						return a_match.form.get_id() == tnam->formID;
					}
				}
			}

			return false;
		}

		template <class Tm, class Tf>
		bool match_form_kw_or_template(
			const Tm& a_match,
			TESForm*  a_form)
		{
			if (IFormCommon::HasKeyword(a_form, a_match.keyword))
			{
				return true;
			}

			if (a_match.flags.test(Tf::kMatchTemplate))
			{
				if (auto armor = a_form->As<TESObjectARMO>())
				{
					if (auto tnam = armor->templateArmor)
					{
						return IFormCommon::HasKeyword(tnam, a_match.keyword);
					}
				}
			}

			return false;
		}*/

		template <class Tm, class Tf>
		constexpr bool match_weather(
			CommonParams& a_params,
			const Tm&     a_match) noexcept
		{
			if (auto current = a_params.get_current_weather())
			{
				if (a_match.form.get_id())
				{
					if (a_match.flags.test(Tf::kNegateMatch1) ==
					    (a_match.form.get_id() == current->formID))
					{
						return false;
					}
				}

				if (a_match.flags.test(Tf::kNegateMatch2) ==
				    a_params.get_weather_class().test_any(a_match.weatherClass))
				{
					return false;
				}

				return true;
			}
			else
			{
				return false;
			}
		}

		template <class Tm, class Tf>
		constexpr bool match_global(
			CommonParams& a_params,
			const Tm&     a_match) noexcept
		{
			auto glob = a_match.form.get_form<TESGlobal>();
			if (!glob)
			{
				return false;
			}

			float matchval = glob->type == TESGlobal::Type::kFloat ?
			                     a_match.f32a :
			                     static_cast<float>(static_cast<std::int64_t>(a_match.f32a));

			return compare(a_match.compOperator, glob->value, matchval);
		}

		template <class Tm>
		constexpr bool match_time_of_day(
			CommonParams& a_params,
			const Tm&     a_match) noexcept
		{
			if (auto tod = a_params.get_time_of_day();
			    tod != Data::TimeOfDay::kNone)
			{
				return (tod & a_match.timeOfDay) != Data::TimeOfDay::kNone;
			}
			else
			{
				return false;
			}
		}

		template <class Tm, class Tf, class Tg>
		constexpr bool match_mount_generic(
			CommonParams& a_params,
			const Tm&     a_match,
			Tg            a_getActorFunc) noexcept
		{
			if (a_match.form.get_id())
			{
				auto& actor = a_getActorFunc(a_params);
				if (!actor)
				{
					return false;
				}

				auto base = actor->GetActorBase();
				if (!base)
				{
					return false;
				}

				if (a_match.flags.test(Tf::kNegateMatch1) ==
				    (a_match.form.get_id() == base->formID))
				{
					return false;
				}
			}

			if (a_match.form2.get_id())  // keyword
			{
				auto& actor = a_getActorFunc(a_params);
				if (!actor)
				{
					return false;
				}

				if (a_match.flags.test(Tf::kNegateMatch3) ==
				    IFormCommon::HasKeyword(actor, a_match.form2))
				{
					return false;
				}
			}

			if (a_match.keyword.get_id())  // actually race
			{
				auto& actor = a_getActorFunc(a_params);
				if (!actor)
				{
					return false;
				}

				auto race = actor->GetRace();
				if (!race)
				{
					return false;
				}

				if (a_match.flags.test(Tf::kNegateMatch2) ==
				    (a_match.keyword.get_id() == race->formID))
				{
					return false;
				}
			}

			return true;
		}

		template <class Tm, class Tf>
		constexpr bool match_mount(
			CommonParams& a_params,
			const Tm&     a_match) noexcept
		{
			if (a_params.is_on_mount())
			{
				if (a_match.flags.test(Tf::kExtraFlag1))
				{
					if (!a_params.is_mounted_actor_horse())
					{
						return false;
					}
				}

				return match_mount_generic<Tm, Tf>(
					a_params,
					a_match,
					[](auto& a_params) -> auto& {
						return a_params.get_mounted_actor();
					});
			}
			else
			{
				return false;
			}
		}

		template <class Tm, class Tf>
		constexpr bool match_mounted_by(
			CommonParams&          a_params,
			const Tm&              a_match,
			const CachedActorData& a_cached) noexcept
		{
			if (a_cached.beingRidden)
			{
				return match_mount_generic<Tm, Tf>(
					a_params,
					a_match,
					[](auto& a_params) -> auto& {
						return a_params.get_mounting_actor();
					});
			}
			else
			{
				return false;
			}
		}

		template <class Tm, class Tf>
		constexpr bool match_idle(
			CommonParams&          a_params,
			const Tm&              a_match,
			const CachedActorData& a_cached) noexcept
		{
			if (const auto idle = a_cached.currentIdle)
			{
				if (const auto fid = a_match.form.get_id())
				{
					if (a_match.flags.test(Tf::kNegateMatch2) ==
					    (fid == idle->formID))
					{
						return false;
					}
				}

				return true;
			}
			else
			{
				return false;
			}
		}

		template <class Tm, class Tf>
		constexpr bool match_skeleton(
			CommonParams& a_params,
			const Tm&     a_match) noexcept
		{
			if (a_match.flags.test(Tf::kExtraFlag1))
			{
				return a_params.objects.GetSkeletonID().signature() == a_match.skeletonSignature;
			}
			else
			{
				return a_params.objects.GetSkeletonID().id() == a_match.skeletonID;
			}
		}

		template <class Tm, class Tf>
		constexpr bool match_faction(
			CommonParams&            a_params,
			const Tm&                a_match,
			const CachedFactionData& a_cached) noexcept
		{
			if (auto form = a_match.form.get_form<TESFaction>())
			{
				auto& data = a_cached.GetFactionContainer();

				auto it = data.find(form);
				if (it == data.end())
				{
					return false;
				}

				if (a_match.flags.test(Tf::kExtraFlag1))
				{
					return compare(
						a_match.compOperator,
						it->second,
						a_match.factionRank);
				}
				else
				{
					return true;
				}
			}

			return false;
		}

		template <class Tm, class Tf>
		constexpr bool match_perk(
			CommonParams&         a_params,
			const Tm&             a_match,
			const CachedPerkData& a_cached) noexcept
		{
			if (const auto formid = a_match.form.get_id())
			{
				auto& data = a_cached.GetPerkContainer();

				auto it = data.find(formid);
				if (it == data.end())
				{
					return false;
				}

				if (a_match.flags.test(Tf::kExtraFlag1))
				{
					return compare(
						a_match.compOperator,
						it->second,
						a_match.perkRank);
				}
				else
				{
					return true;
				}
			}

			return false;
		}

		template <class Tm, class Tf>
		constexpr bool match_effect(
			CommonParams&                 a_params,
			const Tm&                     a_match,
			const CachedActiveEffectData& a_cached) noexcept
		{
			auto& data = a_cached.GetEffectContainer();

			if (a_match.form.get_id())
			{
				const auto rv = a_match.flags.test(Tf::kNegateMatch2);

				if (data.empty())
				{
					return rv;
				}

				const auto form = a_match.form.get_form<EffectSetting>();
				if (!form)
				{
					return rv;
				}

				if (data.find(form) == data.end())
				{
					return rv;
				}

				if (a_match.keyword.get_id())
				{
					if (a_match.flags.test(Tf::kNegateMatch1) ==
					    IFormCommon::HasKeyword(form, a_match.keyword))
					{
						return false;
					}
				}

				return !rv;
			}

			if (a_match.keyword.get_id())
			{
				const auto rv = a_match.flags.test(Tf::kNegateMatch1);

				if (data.empty())
				{
					return rv;
				}

				const auto* const keyword = a_match.keyword.get_form<BGSKeyword>();

				return rv != a_cached.HasEffectWithKeyword(keyword);
			}

			return !data.empty();
		}

		template <class Tm, class Tf>
		constexpr bool match_cell(
			CommonParams& a_params,
			const Tm&     a_match) noexcept
		{
			if (const auto cell = a_params.actor->GetParentCell())
			{
				if (a_match.form.get_id())
				{
					if (a_match.flags.test(Tf::kNegateMatch2) ==
					    (a_match.form.get_id() == cell->formID))
					{
						return false;
					}
				}

				if (a_match.flags.test(Tf::kExtraFlag1))
				{
					if (a_match.flags.test(Tf::kNegateMatch3) == cell->IsInterior())
					{
						return false;
					}
				}

				if (a_match.flags.test(Tf::kExtraFlag2))
				{
					if (a_match.flags.test(Tf::kNegateMatch4) == cell->IsPublicArea())
					{
						return false;
					}
				}

				if ((a_match.lightingTemplateInheritanceFlags &
				     RE::INTERIOR_DATA::Inherit::kAll) != RE::INTERIOR_DATA::Inherit::kNone)
				{
					const stl::flag flags(cell->GetLightingTemplateInheritanceFlags());

					if (a_match.flags.test(Tf::kNegateMatch5) ==
					    flags.test_any(a_match.lightingTemplateInheritanceFlags))
					{
						return false;
					}
				}

				return true;
			}
			else
			{
				return false;
			}
		}

		namespace detail
		{

			template <class Tm>
			SKMP_FORCEINLINE constexpr bool do_var_match(
				const conditionalVariableStorage_t& a_data,
				const Tm&                           a_match) noexcept
			{
				switch (a_data.type)
				{
				case ConditionalVariableType::kInt32:
					switch (a_match.condVarType)
					{
					case ConditionalVariableType::kInt32:
						return compare(a_match.compOperator, a_data.i32, a_match.i32a);
					case ConditionalVariableType::kFloat:
						return compare(a_match.compOperator, static_cast<float>(a_data.i32), a_match.f32a);
					}
					break;
				case ConditionalVariableType::kFloat:
					switch (a_match.condVarType)
					{
					case ConditionalVariableType::kInt32:
						return compare(a_match.compOperator, static_cast<std::int32_t>(a_data.f32), a_match.i32a);
					case ConditionalVariableType::kFloat:
						return compare(a_match.compOperator, a_data.f32, a_match.f32a);
					}
					break;
				case ConditionalVariableType::kForm:
					if (a_match.condVarType == ConditionalVariableType::kForm)
					{
						return compare(a_match.compOperator, a_data.form.get_id().get(), a_match.form2.get_id().get());
					}
					break;
				}

				return false;
			}

			template <class Tm, class Tff>
			constexpr bool do_var_match_all_filtered(
				CommonParams& a_params,
				const Tm&     a_match,
				Tff           a_filter) noexcept
			{
				for (auto& e : get_actor_object_map(a_params))
				{
					if (!a_filter(e))
					{
						continue;
					}

					auto& vdata = e.second.GetVariables();

					auto it = vdata.find(a_match.s0);
					if (it == vdata.end())
					{
						continue;
					}

					if (do_var_match(it->second, a_match))
					{
						return true;
					}
				}

				return false;
			}

			template <class Tm>
			constexpr bool do_var_match_id(
				CommonParams& a_params,
				Game::FormID  a_id,
				const Tm&     a_match) noexcept
			{
				auto& data = get_actor_object_map(a_params);

				auto ita = data.find(a_id);
				if (ita != data.end())
				{
					auto& vdata = ita->second.GetVariables();

					auto it = vdata.find(a_match.s0);
					if (it != vdata.end())
					{
						return do_var_match(it->second, a_match);
					}
				}

				return false;
			}

		}

		template <class Tm, class Tf>
		constexpr bool match_variable(
			CommonParams& a_params,
			const Tm&     a_match) noexcept
		{
			switch (a_match.vcSource)
			{
			case Data::VariableConditionSource::kAny:

				return detail::do_var_match_all_filtered(
					a_params,
					a_match,
					[](auto&) { return true; });

				break;

			case Data::VariableConditionSource::kSelf:

				if (a_match.flags.test(Tf::kNegateMatch2))
				{
					return detail::do_var_match_all_filtered(
						a_params,
						a_match,
						[fid = a_params.objects.GetActorFormID()](auto& a_e) noexcept [[msvc::forceinline]] {
							return a_e.first != fid;
						});
				}
				else
				{
					auto& vdata = a_params.objects.GetVariables();

					auto it = vdata.find(a_match.s0);
					if (it != vdata.end())
					{
						return detail::do_var_match(it->second, a_match);
					}
				}

				break;

			case Data::VariableConditionSource::kActor:

				if (const auto fid = a_match.form.get_id())
				{
					if (a_match.flags.test(Tf::kNegateMatch1))
					{
						return detail::do_var_match_all_filtered(
							a_params,
							a_match,
							[fid](auto& a_e) noexcept [[msvc::forceinline]] {
								return a_e.first != fid;
							});
					}
					else
					{
						return detail::do_var_match_id(a_params, fid, a_match);
					}
				}

				break;

			case Data::VariableConditionSource::kNPC:

				if (const auto fid = a_match.form.get_id())
				{
					if (a_match.flags.test(Tf::kNegateMatch1))
					{
						return detail::do_var_match_all_filtered(
							a_params,
							a_match,
							[fid](auto& a_e) noexcept [[msvc::forceinline]] {
								return a_e.second.GetNPCTemplateFormID() != fid;
							});
					}
					else
					{
						return detail::do_var_match_all_filtered(
							a_params,
							a_match,
							[fid](auto& a_e) noexcept [[msvc::forceinline]] {
								return a_e.second.GetNPCTemplateFormID() == fid;
							});
					}
				}

				break;

			case Data::VariableConditionSource::kRace:

				if (const auto fid = a_match.form.get_id())
				{
					if (a_match.flags.test(Tf::kNegateMatch1))
					{
						return detail::do_var_match_all_filtered(
							a_params,
							a_match,
							[fid](auto& a_e) noexcept [[msvc::forceinline]] {
								return a_e.second.GetRaceFormID() != fid;
							});
					}
					else
					{
						return detail::do_var_match_all_filtered(
							a_params,
							a_match,
							[fid](auto& a_e) noexcept [[msvc::forceinline]] {
								return a_e.second.GetRaceFormID() == fid;
							});
					}
				}

				break;

			case Data::VariableConditionSource::kPlayerHorse:

				if (const auto& actor = a_params.get_last_ridden_player_horse())
				{
					return detail::do_var_match_id(a_params, actor->formID, a_match);
				}

				break;

			case Data::VariableConditionSource::kMountedActor:

				if (const auto& actor = a_params.get_mounted_actor())
				{
					return detail::do_var_match_id(a_params, actor->formID, a_match);
				}

				break;

			case Data::VariableConditionSource::kMountingActor:

				if (const auto& actor = a_params.get_mounting_actor())
				{
					return detail::do_var_match_id(a_params, actor->formID, a_match);
				}

				break;
			}

			return false;
		}

		constexpr TESForm* match_pm_equipped(
			Actor*       a_actor,
			Game::FormID a_form) noexcept
		{
			if (auto pm = a_actor->processManager)
			{
				for (auto e : pm->equippedObject)
				{
					if (e && e->formID == a_form)
					{
						return e;
					}
				}
			}

			return nullptr;
		}

#if defined(IED_ENABLE_CONDITION_EN)
		bool enemies_nearby(CommonParams& a_params) noexcept;
#endif
	}
}