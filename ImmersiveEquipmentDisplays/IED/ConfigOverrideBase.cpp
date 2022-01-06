#include "pch.h"

#include "ConfigOverrideBase.h"
#include "FormCommon.h"

#include "Controller/ObjectManagerData.h"

#include "ProcessParams.h"

namespace IED
{
	namespace Data
	{
		const equipmentOverride_t* configBase_t::get_equipment_override(
			const collectorData_t& a_data,
			CommonParams& a_params) const
		{
			for (auto& e : equipmentOverrides)
			{
				if (match(a_data, e.conditions, a_params, false))
				{
					return std::addressof(e);
				}
			}

			return nullptr;
		}

		const equipmentOverride_t* configBase_t::get_equipment_override(
			const collectorData_t& a_data,
			const slot_container_type& a_slots,
			CommonParams& a_params) const
		{
			for (auto& e : equipmentOverrides)
			{
				if (match(a_data, a_slots, e.conditions, a_params, false))
				{
					return std::addressof(e);
				}
			}

			return nullptr;
		}

		const equipmentOverride_t* configBase_t::get_equipment_override(
			const collectorData_t& a_data,
			const formSlotPair_t& a_checkForm,
			CommonParams& a_params) const
		{
			for (auto& e : equipmentOverrides)
			{
				if (match(a_data, e.conditions, a_checkForm, a_params, false))
				{
					return std::addressof(e);
				}
			}

			return nullptr;
		}

		static TESForm* match_pm_equipped(
			Actor* a_actor,
			Game::FormID a_form)
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

		static constexpr bool is_hand_slot(ObjectSlotExtra a_slot)
		{
			return a_slot != ObjectSlotExtra::kArmor &&
			       a_slot != ObjectSlotExtra::kAmmo;
		}

		static constexpr bool is_valid_form_for_slot(
			TESForm* a_form,
			ObjectSlotExtra a_slot,
			bool a_left)
		{
			return a_left ?
                       ItemData::GetItemSlotLeftExtra(a_form) == a_slot :
                       ItemData::GetItemSlotExtra(a_form) == a_slot;
		}

		static bool match_race(
			const equipmentOverrideCondition_t& a_match,
			TESRace* a_race)
		{
			if (!a_match.form)
			{
				return false;
			}

			if (a_race->formID != a_match.form)
			{
				return false;
			}

			if (a_match.keyword.get_id())
			{
				if (a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch1) ==
				    IFormCommon::HasKeyword(a_race, a_match.keyword))
				{
					return false;
				}
			}

			return true;
		}

		static bool match_furniture(
			CommonParams& a_params,
			const equipmentOverrideCondition_t& a_match)
		{
			if (a_match.form)
			{
				auto furn = a_params.get_furniture();
				if (!furn)
				{
					return false;
				}

				if (a_match.flags.test(Data::EquipmentOverrideConditionFlags::kNegateMatch1) ==
				    (furn->formID == a_match.form))
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

				if (a_match.flags.test(Data::EquipmentOverrideConditionFlags::kNegateMatch2) ==
				    IFormCommon::HasKeyword(furn, a_match.keyword))
				{
					return false;
				}
			}

			if (a_match.flags.test(Data::EquipmentOverrideConditionFlags::kLayingDown))
			{
				return a_params.get_laying_down();
			}
			else
			{
				return a_params.get_using_furniture();
			}
		}

		constexpr bool configBase_t::match_equipped_type(
			const collectorData_t& a_data,
			const equipmentOverrideCondition_t& a_match)
		{
			auto slot = stl::underlying(a_match.slot);
			if (slot >= stl::underlying(ObjectSlotExtra::kMax))
			{
				return false;
			}

			if (is_hand_slot(a_match.slot))
			{
				auto pm = a_data.actor->processManager;
				if (!pm)
				{
					return false;
				}

				auto isLeftSlot = ItemData::IsLeftHandExtraSlot(a_match.slot);

				auto form = pm->equippedObject[isLeftSlot ? ActorProcessManager::kEquippedHand_Left : ActorProcessManager::kEquippedHand_Right];

				if (!form)
				{
					return false;
				}

				if (!is_valid_form_for_slot(form, a_match.slot, isLeftSlot))
				{
					return false;
				}

				if (a_match.form)
				{
					if (a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch1) ==
					    (a_match.form == form->formID))
					{
						return false;
					}
				}

				if (a_match.keyword.get_id())
				{
					if (a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch2) ==
					    IFormCommon::HasKeyword(form, a_match.keyword))
					{
						return false;
					}
				}

				return true;
			}
			else
			{
				if (a_data.slots[slot] == EquippedTypeFlags::kNone)
				{
					return false;
				}

				if (a_match.form)
				{
					auto it = a_data.forms.find(a_match.form);

					auto rv = a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch1);

					if (it == a_data.forms.end())
					{
						return rv;
					}

					if (it->second.extraEquipped.slot != a_match.slot &&
					    it->second.extraEquipped.slotLeft != a_match.slot)
					{
						return rv;
					}

					if (a_match.keyword.get_id())
					{
						if (a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch2) ==
						    IFormCommon::HasKeyword(it->second.form, a_match.keyword))
						{
							return false;
						}
					}

					return !rv;
				}
				else
				{
					if (a_match.keyword.get_id())
					{
						if (a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch2) ==
						    has_keyword(a_match.keyword, a_match.slot, a_data))
						{
							return false;
						}
					}

					return true;
				}
			}

			return false;
		}

		bool configBase_t::match_equipped_form(
			const collectorData_t& a_data,
			const equipmentOverrideCondition_t& a_match)
		{
			auto form = match_pm_equipped(a_data.actor, a_match.form);
			if (!form)
			{
				auto it = a_data.forms.find(a_match.form);
				if (it == a_data.forms.end())
				{
					return false;
				}

				form = it->second.form;

				if (!it->second.equipped &&
				    !it->second.equippedLeft)
				{
					return false;
				}
			}

			if (a_match.keyword.get_id())
			{
				return a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch1) !=
				       has_keyword(a_match.keyword, form);
			}
			else
			{
				return true;
			}
		}

		constexpr bool configBase_t::match(
			const collectorData_t& a_data,
			const equipmentOverrideCondition_t& a_match,
			CommonParams& a_params)
		{
			switch (a_match.fbf.type)
			{
			case EquipmentOverrideConditionType::Type:

				if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
				{
					return match_equipped_type(a_data, a_match);
				}

				break;

			case EquipmentOverrideConditionType::Keyword:

				if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
				{
					if (!a_match.keyword.get_id())
					{
						return false;
					}

					return has_keyword(a_match.keyword, a_data);
				}

				break;

			case EquipmentOverrideConditionType::Form:

				if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
				{
					if (!a_match.form)
					{
						return false;
					}

					return match_equipped_form(a_data, a_match);
				}

				break;

			case EquipmentOverrideConditionType::Race:

				return match_race(a_match, a_data.race);

			case EquipmentOverrideConditionType::Furniture:

				return match_furniture(a_params, a_match);
			}

			return false;
		}

		constexpr bool configBase_t::match(
			const collectorData_t& a_data,
			const equipmentOverrideConditionList_t& a_matches,
			CommonParams& a_params,
			bool a_default)
		{
			bool result = a_default;

			for (auto& f : a_matches)
			{
				result = match(a_data, f, a_params);

				if (f.flags.test(EquipmentOverrideConditionFlags::kNot))
				{
					result = !result;
				}

				if (f.flags.test(EquipmentOverrideConditionFlags::kAnd))
				{
					if (!result)
					{
						return false;
					}
				}
				else
				{
					if (result)
					{
						return true;
					}
				}
			}

			return result;
		}

		constexpr bool configBase_t::match(
			const collectorData_t& a_cdata,
			const slot_container_type& a_data,
			const equipmentOverrideCondition_t& a_match,
			CommonParams& a_params)
		{
			switch (a_match.fbf.type)
			{
			case EquipmentOverrideConditionType::Type:
				{
					if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchAll))
					{
						return false;
					}

					std::uint32_t result = 0;
					std::uint32_t min = a_match.flags.test(EquipmentOverrideConditionFlags::kMatchAll) &&
					                            !a_match.flags.test(EquipmentOverrideConditionFlags::kMatchCategoryOperOR) ?
                                            2u :
                                            1u;

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
					{
						result += match_equipped_type(a_cdata, a_match);

						if (result == min)
						{
							return true;
						}
					}

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchSlots))
					{
						auto i = stl::underlying(ItemData::ExtraSlotToSlot(a_match.slot));
						if (i >= stl::underlying(ObjectSlot::kMax))
						{
							return false;
						}

						auto form = a_data[i].GetFormIfActive();
						if (!form)
						{
							return false;
						}

						if (a_match.form)
						{
							if (a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch1) ==
							    (form->formID == a_match.form))
							{
								return false;
							}
						}

						if (a_match.keyword.get_id())
						{
							if (a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch2) ==
							    IFormCommon::HasKeyword(form, a_match.keyword))
							{
								return false;
							}
						}

						result++;
					}

					return result == min;
				}
			case EquipmentOverrideConditionType::Keyword:
				{
					if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchAll))
					{
						return false;
					}

					if (!a_match.keyword.get_id())
					{
						return false;
					}

					std::uint32_t result = 0;
					std::uint32_t min = a_match.flags.test(EquipmentOverrideConditionFlags::kMatchAll) &&
					                            !a_match.flags.test(EquipmentOverrideConditionFlags::kMatchCategoryOperOR) ?
                                            2u :
                                            1u;

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
					{
						result += has_keyword(a_match.keyword, a_cdata);

						if (result == min)
						{
							return true;
						}
					}

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchSlots))
					{
						result += has_keyword(a_match.keyword, a_data);
					}

					return result == min;
				}
			case EquipmentOverrideConditionType::Form:
				{
					if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchAll))
					{
						return false;
					}

					if (!a_match.form)
					{
						return false;
					}

					std::uint32_t result = 0;
					std::uint32_t min = a_match.flags.test(EquipmentOverrideConditionFlags::kMatchAll) &&
					                            !a_match.flags.test(EquipmentOverrideConditionFlags::kMatchCategoryOperOR) ?
                                            2u :
                                            1u;

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
					{
						result += match_equipped_form(a_cdata, a_match);

						if (result == min)
						{
							return true;
						}
					}

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchSlots))
					{
						auto form = match_slot_form(a_data, a_match);
						if (!form)
						{
							return false;
						}

						if (a_match.keyword.get_id())
						{
							if (a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch1) ==
							    IFormCommon::HasKeyword(form, a_match.keyword))
							{
								return false;
							}
						}

						result++;
					}

					return result == min;
				}
			case EquipmentOverrideConditionType::Race:

				return match_race(a_match, a_cdata.race);

			case EquipmentOverrideConditionType::Furniture:

				return match_furniture(a_params, a_match);
			}

			return false;
		}

		constexpr bool configBase_t::match(
			const collectorData_t& a_cdata,
			const slot_container_type& a_data,
			const equipmentOverrideConditionList_t& a_matches,
			CommonParams& a_params,
			bool a_default)
		{
			bool result = a_default;

			for (auto& f : a_matches)
			{
				result = match(a_cdata, a_data, f, a_params);

				if (f.flags.test(EquipmentOverrideConditionFlags::kNot))
				{
					result = !result;
				}

				if (f.flags.test(EquipmentOverrideConditionFlags::kAnd))
				{
					if (!result)
					{
						return false;
					}
				}
				else
				{
					if (result)
					{
						return true;
					}
				}
			}

			return result;
		}

		constexpr bool configBase_t::match(
			const collectorData_t& a_data,
			const equipmentOverrideCondition_t& a_match,
			const formSlotPair_t& a_checkForm,
			CommonParams& a_params)
		{
			switch (a_match.fbf.type)
			{
			case EquipmentOverrideConditionType::Type:
				{
					if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchAll))
					{
						return false;
					}

					std::uint32_t result = 0;
					std::uint32_t min = a_match.flags.test(EquipmentOverrideConditionFlags::kMatchAll) &&
					                            !a_match.flags.test(EquipmentOverrideConditionFlags::kMatchCategoryOperOR) ?
                                            2u :
                                            1u;

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
					{
						result += match_equipped_type(a_data, a_match);

						if (result == min)
						{
							return true;
						}
					}

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchSlots))
					{
						if (a_match.slot != a_checkForm.slot)
						{
							return false;
						}

						if (a_match.form)
						{
							if (a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch1) ==
							    (a_match.form == a_checkForm.form->formID))
							{
								return false;
							}
						}

						if (a_match.keyword.get_id())
						{
							if (a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch2) ==
							    has_keyword(a_match.keyword, a_checkForm.form))
							{
								return false;
							}
						}

						result++;
					}

					return result == min;
				}

			case EquipmentOverrideConditionType::Keyword:
				{
					if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchAll))
					{
						return false;
					}

					if (!a_match.keyword.get_id())
					{
						return false;
					}

					std::uint32_t result = 0;
					std::uint32_t min = a_match.flags.test(EquipmentOverrideConditionFlags::kMatchAll) &&
					                            !a_match.flags.test(EquipmentOverrideConditionFlags::kMatchCategoryOperOR) ?
                                            2u :
                                            1u;

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
					{
						result += has_keyword(a_match.keyword, a_data);

						if (result == min)
						{
							return true;
						}
					}

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchSlots))
					{
						result += has_keyword(a_match.keyword, a_checkForm.form);
					}

					return result == min;
				}
			case EquipmentOverrideConditionType::Form:
				{
					if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchAll))
					{
						return false;
					}

					if (!a_match.form)
					{
						return false;
					}

					std::uint32_t result = 0;
					std::uint32_t min = a_match.flags.test(EquipmentOverrideConditionFlags::kMatchAll) &&
					                            !a_match.flags.test(EquipmentOverrideConditionFlags::kMatchCategoryOperOR) ?
                                            2u :
                                            1u;

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
					{
						result += match_equipped_form(a_data, a_match);

						if (result == min)
						{
							return true;
						}
					}

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchSlots))
					{
						if (a_match.form != a_checkForm.form->formID)
						{
							return false;
						}

						if (a_match.keyword.get_id())
						{
							if (a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch1) ==
							    has_keyword(a_match.keyword, a_checkForm.form))
							{
								return false;
							}
						}

						result++;
					}

					return result == min;
				}
			case EquipmentOverrideConditionType::Race:

				return match_race(a_match, a_data.race);

			case EquipmentOverrideConditionType::Furniture:

				return match_furniture(a_params, a_match);
			}

			return false;
		}

		constexpr bool configBase_t::match(
			const collectorData_t& a_data,
			const equipmentOverrideConditionList_t& a_matches,
			const formSlotPair_t& a_checkForm,
			CommonParams& a_params,
			bool a_default)
		{
			bool result = a_default;

			for (auto& f : a_matches)
			{
				result = match(a_data, f, a_checkForm, a_params);

				if (f.flags.test(EquipmentOverrideConditionFlags::kNot))
				{
					result = !result;
				}

				if (f.flags.test(EquipmentOverrideConditionFlags::kAnd))
				{
					if (!result)
					{
						return false;
					}
				}
				else
				{
					if (result)
					{
						return true;
					}
				}
			}

			return result;
		}

		bool configBase_t::has_keyword(
			const configCachedForm_t& a_keyword,
			const collectorData_t& a_data)
		{
			if (auto form = a_keyword.get_form())
			{
				if (auto keyword = form->As<BGSKeyword>())
				{
					if (auto pm = a_data.actor->processManager)
					{
						for (auto e : pm->equippedObject)
						{
							if (e && IFormCommon::HasKeyword(e, keyword))
							{
								return true;
							}
						}
					}

					for (auto& e : a_data.forms)
					{
						if (e.second.equipped || e.second.equippedLeft)
						{
							if (IFormCommon::HasKeyword(e.second.form, keyword))
							{
								return true;
							}
						}
					}
				}
			}

			return false;
		}

		bool configBase_t::has_keyword(
			const configCachedForm_t& a_keyword,
			const slot_container_type& a_data)
		{
			if (auto form = a_keyword.get_form())
			{
				if (auto keyword = form->As<BGSKeyword>())
				{
					for (auto& e : a_data)
					{
						if (auto iform = e.GetFormIfActive())
						{
							if (IFormCommon::HasKeyword(iform, keyword))
							{
								return true;
							}
						}
					}
				}
			}

			return false;
		}

		bool configBase_t::has_keyword(
			const configCachedForm_t& a_keyword,
			TESForm* a_form)
		{
			if (auto form = a_keyword.get_form())
			{
				if (auto keyword = form->As<BGSKeyword>())
				{
					if (IFormCommon::HasKeyword(a_form, keyword))
					{
						return true;
					}
				}
			}

			return false;
		}

		bool configBase_t::has_keyword(
			const configCachedForm_t& a_keyword,
			ObjectSlotExtra a_slot,
			const collectorData_t& a_data)
		{
			if (auto form = a_keyword.get_form())
			{
				if (auto keyword = form->As<BGSKeyword>())
				{
					for (auto& e : a_data.forms)
					{
						if (e.second.extraEquipped.slot == a_slot ||
						    e.second.extraEquipped.slotLeft == a_slot)
						{
							if (IFormCommon::HasKeyword(e.second.form, keyword))
							{
								return true;
							}
						}
					}
				}
			}

			return false;
		}

		bool configBase_t::has_keyword(
			const configCachedForm_t& a_keyword,
			ObjectSlot a_slot,
			const slot_container_type& a_data)
		{
			if (auto form = a_keyword.get_form())
			{
				if (auto keyword = form->As<BGSKeyword>())
				{
					auto& slot = a_data[stl::underlying(a_slot)];

					if (auto iform = slot.GetFormIfActive())
					{
						if (IFormCommon::HasKeyword(iform, keyword))
						{
							return true;
						}
					}
				}
			}

			return false;
		}

		TESForm* configBase_t::match_slot_form(
			const slot_container_type& a_data,
			const equipmentOverrideCondition_t& a_match)
		{
			for (auto& e : a_data)
			{
				if (auto form = e.GetFormIfActive())
				{
					if (form->formID == a_match.form)
					{
						return form;
					}
				}
			}

			return nullptr;
		}

		bool configBase_t::has_equipment_override_form(
			Game::FormID a_formid) const
		{
			/*auto it = std::find_if(
				equipmentOverrides.begin(),
				equipmentOverrides.end(),
				[a_formid](auto& a_entry) { return a_formid == a_entry.form; });

			return it != equipmentOverrides.end();*/
			return false;
		}

		bool configBase_t::has_equipment_override_type(ObjectTypeExtra a_type) const
		{
			/*auto it = std::find_if(
				equipmentOverrides.begin(),
				equipmentOverrides.end(),
				[a_type](auto& a_entry) { return a_type == a_entry.type; });

			return it != equipmentOverrides.end();*/
			return false;
		}

		bool configBaseFiltersHolder_t::run_filters(const processParams_t& a_params) const
		{
			if (filters)
			{
				return filters->actorFilter.test(a_params.actor->formID) &&
				       filters->npcFilter.test(a_params.npc->formID) &&
				       filters->raceFilter.test(a_params.race->formID);
			}
			else
			{
				return true;
			}
		}
	}
}