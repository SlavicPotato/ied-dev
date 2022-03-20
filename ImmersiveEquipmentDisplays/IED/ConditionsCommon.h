#pragma once

#include "CommonParams.h"
#include "ConfigData.h"
#include "FormCommon.h"
#include "WeatherClassificationFlags.h"

#include "IED/Data.h"

namespace IED
{
	namespace Conditions
	{
		template <class Tm, class Tf>
		constexpr bool match_extra(
			CommonParams& a_params,
			const Tm&     a_match)
		{
			switch (a_match.extraCondType)
			{
			case Data::ExtraConditionType::kCanDualWield:
				return a_params.can_dual_wield();
			case Data::ExtraConditionType::kIsDead:
				return a_params.get_actor_dead();
			case Data::ExtraConditionType::kInInterior:
				return a_params.get_in_interior();
			case Data::ExtraConditionType::kIsPlayerTeammate:
				return a_params.is_player_teammate();
			case Data::ExtraConditionType::kIsGuard:
				return a_params.actor->IsGuard();
			case Data::ExtraConditionType::kIsMount:
				return a_params.actor->IsMount();
			case Data::ExtraConditionType::kShoutEquipped:
				return match_form_with_id<Tm, Tf>(a_match, a_params.actor->equippedShout);
			case Data::ExtraConditionType::kInMerchantFaction:
				return match_form_with_id<Tm, Tf>(a_match, a_params.actor->vendorFaction);
			case Data::ExtraConditionType::kCombatStyle:
				return match_form_with_id<Tm, Tf>(a_match, a_params.get_combat_style());
			case Data::ExtraConditionType::kClass:
				return match_form_with_id<Tm, Tf>(a_match, a_params.npc->GetClass());
			default:
				return false;
			}
		}

		bool match_form(
			Game::FormID a_formid,
			TESForm*     a_form);

		inline constexpr bool is_hand_slot(Data::ObjectSlotExtra a_slot)
		{
			return a_slot != Data::ObjectSlotExtra::kArmor &&
			       a_slot != Data::ObjectSlotExtra::kAmmo;
		}

		inline constexpr bool is_valid_form_for_slot(
			TESForm*              a_form,
			Data::ObjectSlotExtra a_slot,
			bool                  a_left)
		{
			return a_left ?
			           Data::ItemData::GetItemSlotLeftExtra(a_form) == a_slot :
                       Data::ItemData::GetItemSlotExtra(a_form) == a_slot;
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

				return true;
			}
			else
			{
				return false;
			}
		}

		template <class Tm, class Tf>
		constexpr bool match_worldspace(
			CommonParams& a_params,
			const Tm&     a_match)
		{
			if (auto current = a_params.get_worldspace())
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
			CommonParams& a_params,
			const Tm&     a_match)
		{
			if (auto current = a_params.actor->GetCurrentPackage())
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

		template <class Tm, class Tf>
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
		}

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
                                 static_cast<float>(static_cast<long>(a_match.f32a));

			bool result;

			switch (a_match.compOperator)
			{
			case Data::ComparisonOperator::kEqual:
				result = glob->value == matchval;
				break;
			case Data::ComparisonOperator::kNotEqual:
				result = glob->value != matchval;
				break;
			case Data::ComparisonOperator::kGreater:
				result = glob->value > matchval;
				break;
			case Data::ComparisonOperator::kLower:
				result = glob->value < matchval;
				break;
			case Data::ComparisonOperator::kGreaterOrEqual:
				result = glob->value >= matchval;
				break;
			case Data::ComparisonOperator::kLowerOrEqual:
				result = glob->value <= matchval;
				break;
			default:
				return false;
			}

			return result;
		}

		bool is_ammo_bolt(TESForm* a_form);

	}
}