#pragma once

#include "CommonParams.h"
#include "ConfigOverrideCommon.h"
#include "FormCommon.h"

#include "IED/Data.h"

namespace IED
{
	namespace Conditions
	{
		bool match_extra(
			CommonParams& a_params,
			Data::ExtraConditionType a_type);

		bool match_form(
			Game::FormID a_formid,
			TESForm* a_form);

		inline constexpr bool is_hand_slot(Data::ObjectSlotExtra a_slot)
		{
			return a_slot != Data::ObjectSlotExtra::kArmor &&
			       a_slot != Data::ObjectSlotExtra::kAmmo;
		}

		inline constexpr bool is_valid_form_for_slot(
			TESForm* a_form,
			Data::ObjectSlotExtra a_slot,
			bool a_left)
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
			BGSKeyword* a_keyword,
			BGSLocation* a_matched);

		bool is_in_location(
			BGSLocation* a_current,
			BGSKeyword* a_keyword);

		template <class Tm, class Tf, class Tp>
		constexpr bool match_biped(
			CommonParams& a_params,
			const Tm& a_match,
			Tp a_post)
		{
			if (a_match.bipedSlot >= Biped::kTotal)
			{
				return false;
			}

			auto biped = a_params.get_biped();
			if (!biped)
			{
				return false;
			}

			auto& e = biped->objects[a_match.bipedSlot];

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

			a_post(form);

			return true;
		}

		template <class Tm, class Tf>
		constexpr bool match_race(
			CommonParams& a_params,
			const Tm& a_match)
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

			return true;
		}

		template <class Tm, class Tf>
		constexpr bool match_furniture(
			CommonParams& a_params,
			const Tm& a_match)
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
			const Tm& a_match)
		{
			if (auto current = a_params.get_location())
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
						    Conditions::is_in_location(current, location))
						{
							return false;
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
                                 Conditions::is_in_location(current, keyword, location) :
                                 Conditions::is_in_location(current, keyword)))
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

	}
}