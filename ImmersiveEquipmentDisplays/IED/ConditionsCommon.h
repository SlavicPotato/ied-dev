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
		template <class Tm, class Tf>
		constexpr bool match_extra(
			CommonParams&          a_params,
			const Tm&              a_match,
			const CachedActorData& a_cached)
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
				return !a_params.is_player() && a_cached.flags1.test(Actor::Flags1::kPlayerTeammate);
			case Data::ExtraConditionType::kIsGuard:
				return a_cached.flags1.test(Actor::Flags1::kGuard);
			case Data::ExtraConditionType::kIsMount:
				return a_params.actor->IsMount();
			case Data::ExtraConditionType::kShoutEquipped:
				return match_form_with_id<Tm, Tf>(a_match, a_params.actor->selectedPower);
			/*case Data::ExtraConditionType::kInMerchantFaction:
				return match_form_with_id<Tm, Tf>(a_match, a_params.actor->vendorFaction);*/
			case Data::ExtraConditionType::kCombatStyle:
				return match_form_with_id<Tm, Tf>(a_match, a_params.get_combat_style());
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
				return a_params.is_player();
			case Data::ExtraConditionType::kBribedByPlayer:
				return !a_params.is_player() && a_cached.flags2.test(Actor::Flags2::kBribedByPlayer);
			case Data::ExtraConditionType::kAngryWithPlayer:
				return !a_params.is_player() && a_cached.flags2.test(Actor::Flags2::kAngryWithPlayer);
			case Data::ExtraConditionType::kEssential:
				return a_cached.flags2.test(Actor::Flags2::kEssential);
			case Data::ExtraConditionType::kProtected:
				return a_cached.flags2.test(Actor::Flags2::kProtected);
			case Data::ExtraConditionType::kSitting:
				return a_cached.sitting;
			case Data::ExtraConditionType::kSleeping:
				return a_cached.sleeping;
			case Data::ExtraConditionType::kBeingRidden:
				return a_cached.beingRidden;
			case Data::ExtraConditionType::kWeaponDrawn:
				return a_cached.weaponDrawn;
			case Data::ExtraConditionType::kRandomPercent:
				return match_random_percent(a_params, a_match, a_match.percent);
			case Data::ExtraConditionType::kNodeMonitor:
				return check_node_monitor_value(a_params, a_match.uid);
			case Data::ExtraConditionType::kArrested:
				return a_cached.arrested;
			case Data::ExtraConditionType::kIsChild:
				return a_params.actor->IsChild();
			case Data::ExtraConditionType::kInKillmove:
				return a_cached.flags2.test(Actor::Flags2::kIsInKillMove);
			case Data::ExtraConditionType::kIsUnconscious:
				return a_cached.unconscious;
			case Data::ExtraConditionType::kIsPlayerLastRiddenMount:
				return is_player_last_ridden_mount(a_params);
			default:
				return false;
			}
		}

		inline constexpr bool match_form(
			Game::FormID a_formid,
			TESForm*     a_form) noexcept
		{
			return a_formid && a_form->formID == a_formid;
		}

		inline constexpr bool is_hand_slot(Data::ObjectSlotExtra a_slot) noexcept
		{
			return a_slot != Data::ObjectSlotExtra::kArmor &&
			       a_slot != Data::ObjectSlotExtra::kAmmo;
		}

		inline constexpr bool is_valid_form_for_slot(
			TESForm*              a_form,
			Data::ObjectSlotExtra a_slot,
			bool                  a_left) noexcept
		{
			return a_left ?
			           Data::ItemData::GetItemSlotLeftExtra(a_form) == a_slot :
                       Data::ItemData::GetItemSlotExtra(a_form) == a_slot;
		}

		inline constexpr bool is_ammo_bolt(TESForm* a_form) noexcept
		{
			if (auto ammo = a_form->As<TESAmmo>())
			{
				return ammo->isBolt();
			}
			else
			{
				return false;
			}
		}

		inline constexpr bool is_object_visible(NiPointer<NiAVObject>& a_object) noexcept
		{
			return a_object && a_object->IsVisible();
		}

		bool is_in_location(
			BGSLocation* a_current,
			BGSLocation* a_loc);

		bool is_in_location(
			BGSLocation* a_current,
			BGSKeyword*  a_keyword,
			BGSLocation* a_matched);

		bool is_in_location(
			BGSLocation* a_current,
			BGSKeyword*  a_keyword);

		bool match_worldspace(
			TESWorldSpace* a_current,
			Game::FormID   a_wsId,
			bool           a_matchParent);

		template <class Tm, class Tf, class Tp>
		constexpr bool match_biped(
			CommonParams& a_params,
			const Tm&     a_match,
			Tp            a_post)
		{
			if (a_match.bipedSlot >= BIPED_OBJECT::kTotal)
			{
				return false;
			}

			auto biped = a_params.get_biped();
			if (!biped)
			{
				return false;
			}

			auto& e = biped->get_object(a_match.bipedSlot);

			auto form = e.item;
			if (!form || e.addon == form)
			{
				return false;
			}

			if (a_match.flags.test(Tf::kExtraFlag2))
			{
				if (auto skin = a_params.get_actor_skin())
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
		constexpr bool match_race(
			CommonParams& a_params,
			const Tm&     a_match)
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
			const Tm&     a_match)
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

			if (a_match.flags.test(Tf::kExtraFlag1))
			{
				return a_params.get_laying_down();
			}
			else
			{
				return a_params.get_using_furniture();
			}
		}

		template <class Tm, class Tf>
		constexpr bool match_location(
			CommonParams& a_params,
			const Tm&     a_match)
		{
			if (auto current = a_params.get_current_location())
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
			const CachedActorData& a_cached)
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
			const CachedActorData& a_cached)
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
		bool match_form_with_id(
			const Tm& a_match,
			TESForm*  a_form)
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
			const Tm&     a_match)
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

		template <class Tv, class Tm>
		inline constexpr bool compare(
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
		constexpr bool match_global(
			CommonParams& a_params,
			const Tm&     a_match)
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
			const Tm&     a_match)
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

		template <class Tm, class Tf>
		constexpr bool match_mount(
			CommonParams& a_params,
			const Tm&     a_match)
		{
			if (a_params.is_on_mount())
			{
				if (a_match.form.get_id())
				{
					auto& mountedActor = a_params.get_mounted_actor();
					if (!mountedActor)
					{
						return false;
					}

					auto base = mountedActor->GetActorBase();
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

				if (a_match.keyword.get_id())  // actually race
				{
					auto& mountedActor = a_params.get_mounted_actor();
					if (!mountedActor)
					{
						return false;
					}

					auto race = mountedActor->GetRace();
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
			else
			{
				return false;
			}
		}

		template <class Tm, class Tf>
		constexpr bool match_mounted_by(
			CommonParams&          a_params,
			const Tm&              a_match,
			const CachedActorData& a_cached)
		{
			if (a_cached.beingRidden)
			{
				if (a_match.form.get_id())
				{
					auto& mountedByActor = a_params.get_mounted_by_actor();
					if (!mountedByActor)
					{
						return false;
					}

					auto base = mountedByActor->GetActorBase();
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

				if (a_match.keyword.get_id())  // actually race
				{
					auto& mountedByActor = a_params.get_mounted_by_actor();
					if (!mountedByActor)
					{
						return false;
					}

					auto race = mountedByActor->GetRace();
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
			else
			{
				return false;
			}
		}

		template <class Tm, class Tf>
		constexpr bool match_idle(
			CommonParams&          a_params,
			const Tm&              a_match,
			const CachedActorData& a_cached)
		{
			const auto idle = a_cached.currentIdle;
			const auto cfid = idle ? idle->formID : Game::FormID{};

			return a_match.flags.test(Tf::kNegateMatch2) !=
			       (a_match.form.get_id() == cfid);
		}

		template <class Tm, class Tf>
		constexpr bool match_skeleton(
			CommonParams& a_params,
			const Tm&     a_match) noexcept
		{
			if (a_match.flags.test(Tf::kExtraFlag1))
			{
				return get_skeleton_id(a_params).signature() == a_match.skeletonSignature;
			}
			else
			{
				return get_skeleton_id(a_params).id() == a_match.skeletonID;
			}
		}

		template <class Tm, class Tf>
		constexpr bool match_faction(
			CommonParams&            a_params,
			const Tm&                a_match,
			const CachedFactionData& a_cached)
		{
			if (a_match.form.get_id())
			{
				auto& data = a_cached.GetFactionContainer();

				auto it = data.find(a_match.form.get_id());
				if (it == data.end())
				{
					return false;
				}

				if (a_match.flags.test(Tf::kExtraFlag1))
				{
					return compare(a_match.compOperator, it->second, a_match.factionRank);
				}
				else
				{
					return true;
				}
			}

			return false;
		}

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
			case ConditionalVariableType::kFloat:
				switch (a_match.condVarType)
				{
				case ConditionalVariableType::kInt32:
					return compare(a_match.compOperator, static_cast<std::int32_t>(a_data.f32), a_match.i32a);
				case ConditionalVariableType::kFloat:
					return compare(a_match.compOperator, a_data.f32, a_match.f32a);
				}
			}

			return false;
		}

		template <class Tm, class Tff>
		constexpr bool do_var_match_all_filtered(
			CommonParams& a_params,
			const Tm&     a_match,
			Tff           a_filter)
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

				if (!it->second.second)
				{
					continue;
				}

				if (do_var_match(it->second.first, a_match))
				{
					return true;
				}
			}

			return false;
		}

		template <class Tm, class Tf>
		constexpr bool match_variable(
			CommonParams& a_params,
			const Tm&     a_match)
		{
			switch (a_match.vcTarget)
			{
			case Data::VariableConditionTarget::kAll:
				{
					return do_var_match_all_filtered(
						a_params,
						a_match,
						[](auto&) { return true; });
				}
				break;

			case Data::VariableConditionTarget::kSelf:
				{
					if (a_match.flags.test(Tf::kNegateMatch2))
					{
						return do_var_match_all_filtered(
							a_params,
							a_match,
							[fid = a_params.objects.GetActorFormID()](auto& a_e) {
								return a_e.first != fid;
							});
					}
					else
					{
						auto& vdata = a_params.objects.GetVariables();

						auto it = vdata.find(a_match.s0);
						if (it != vdata.end())
						{
							if (it->second.second)
							{
								return do_var_match(it->second.first, a_match);
							}
						}
					}
				}
				break;

			case Data::VariableConditionTarget::kActor:
				{
					if (auto fid = a_match.form.get_id())
					{
						if (a_match.flags.test(Tf::kNegateMatch1))
						{
							return do_var_match_all_filtered(
								a_params,
								a_match,
								[fid](auto& a_e) {
									return a_e.first != fid;
								});
						}
						else
						{
							auto& data = get_actor_object_map(a_params);

							auto ita = data.find(fid);
							if (ita != data.end())
							{
								auto& vdata = ita->second.GetVariables();

								auto it = vdata.find(a_match.s0);
								if (it != vdata.end())
								{
									if (it->second.second)
									{
										return do_var_match(it->second.first, a_match);
									}
								}
							}
						}
					}
				}

				break;

			case Data::VariableConditionTarget::kNPC:
				{
					if (auto fid = a_match.form.get_id())
					{
						if (a_match.flags.test(Tf::kNegateMatch1))
						{
							return do_var_match_all_filtered(
								a_params,
								a_match,
								[fid](auto& a_e) {
									return a_e.second.GetNPCTemplateFormID() != fid;
								});
						}
						else
						{
							return do_var_match_all_filtered(
								a_params,
								a_match,
								[fid](auto& a_e) {
									return a_e.second.GetNPCTemplateFormID() == fid;
								});
						}
					}
				}

				break;

			case Data::VariableConditionTarget::kRace:
				{
					if (auto fid = a_match.form.get_id())
					{
						if (a_match.flags.test(Tf::kNegateMatch1))
						{
							return do_var_match_all_filtered(
								a_params,
								a_match,
								[fid](auto& a_e) {
									return a_e.second.GetRaceFormID() != fid;
								});
						}
						else
						{
							return do_var_match_all_filtered(
								a_params,
								a_match,
								[fid](auto& a_e) {
									return a_e.second.GetRaceFormID() == fid;
								});
						}
					}
				}

				break;
			}

			return false;
		}

		const ActorObjectMap& get_actor_object_map(CommonParams& a_params);

		const SkeletonID& get_skeleton_id(CommonParams& a_params) noexcept;
		bool              is_in_first_person(CommonParams& a_params) noexcept;
		bool              check_node_monitor_value(CommonParams& a_params, std::uint32_t a_uid) noexcept;
		bool              is_player_last_ridden_mount(CommonParams& a_params) noexcept;

		bool match_random_percent(
			CommonParams&   a_params,
			const luid_tag& a_luid,
			float           a_percent) noexcept;

#if defined(IED_ENABLE_CONDITION_EN)
		bool enemies_nearby(CommonParams& a_params) noexcept;
#endif
	}
}