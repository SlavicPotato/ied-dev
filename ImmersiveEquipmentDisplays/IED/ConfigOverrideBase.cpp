#include "pch.h"

#include "ConfigConditionsCommon.h"
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
			CommonParams&          a_params) const
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
			const collectorData_t&     a_data,
			const slot_container_type& a_slots,
			CommonParams&              a_params) const
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
			const formSlotPair_t&  a_checkForm,
			CommonParams&          a_params) const
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

		const configEffectShaderHolder_t* configBase_t::get_effect_shader(
			const collectorData_t& a_data,
			CommonParams&          a_params) const
		{
			for (auto& e : effectShaders.data)
			{
				if (!e.enabled())
				{
					continue;
				}

				if (match(a_data, e.conditions, a_params, true))
				{
					return std::addressof(e);
				}
			}

			return nullptr;
		}

		const configEffectShaderHolder_t* configBase_t::get_effect_shader(
			const collectorData_t&     a_data,
			const slot_container_type& a_slots,
			CommonParams&              a_params) const
		{
			for (auto& e : effectShaders.data)
			{
				if (!e.enabled())
				{
					continue;
				}

				if (match(a_data, a_slots, e.conditions, a_params, true))
				{
					return std::addressof(e);
				}
			}

			return nullptr;
		}

		const configEffectShaderHolder_t* configBase_t::get_effect_shader(
			const collectorData_t& a_data,
			const formSlotPair_t&  a_checkForm,
			CommonParams&          a_params) const
		{
			for (auto& e : effectShaders.data)
			{
				if (!e.enabled())
				{
					continue;
				}

				if (match(a_data, e.conditions, a_checkForm, a_params, true))
				{
					return std::addressof(e);
				}
			}

			return nullptr;
		}

		static TESForm* match_pm_equipped(
			Actor*       a_actor,
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

		static bool match_quest(
			CommonParams&                       a_params,
			const equipmentOverrideCondition_t& a_match)
		{
			auto form = a_match.keyword.get_form<TESQuest>();
			if (!form)
			{
				return false;
			}

			if (a_match.questCondType == Data::QuestConditionType::kComplete)
			{
				return form->questData.flags.test_any(TESQuest::QuestFlag::kCompleted);
			}
			else
			{
				return false;
			}
		}

		constexpr bool configBase_t::match_equipped_type(
			const collectorData_t&              a_data,
			const equipmentOverrideCondition_t& a_match)
		{
			auto slot = stl::underlying(a_match.slot);
			if (slot >= stl::underlying(ObjectSlotExtra::kMax))
			{
				return false;
			}

			if (Conditions::is_hand_slot(a_match.slot))
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

				if (!Conditions::is_valid_form_for_slot(form, a_match.slot, isLeftSlot))
				{
					return false;
				}

				if (a_match.form.get_id())
				{
					if (a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch1) ==
					    (a_match.form.get_id() == form->formID))
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
				if (a_data.equippedTypeFlags[slot] == InventoryPresenceFlags::kNone)
				{
					return false;
				}

				if (a_match.form.get_id())
				{
					auto it = a_data.forms.find(a_match.form.get_id());

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
						    has_keyword_equipped(a_match.keyword, a_match.slot, a_data))
						{
							return false;
						}
					}

					return true;
				}
			}

			return false;
		}

		bool configBase_t::match_carried_type(
			const collectorData_t&              a_data,
			const equipmentOverrideCondition_t& a_match)
		{
			auto type = Data::ItemData::GetTypeFromSlotExtra(a_match.slot);

			if (type >= ObjectTypeExtra::kMax)
			{
				return false;
			}

			if (a_data.typeCount[stl::underlying(type)] < 1)
			{
				return false;
			}

			if (a_match.form.get_id())
			{
				auto it = a_data.forms.find(a_match.form.get_id());

				auto rv = a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch1);

				if (it == a_data.forms.end())
				{
					return rv;
				}

				if (it->second.count < 1 ||
				    it->second.typeExtra != type)
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
					    has_keyword_carried(a_match.keyword, type, a_data))
					{
						return false;
					}
				}

				return true;
			}

			return false;
		}

		bool configBase_t::match_equipped_form(
			const collectorData_t&              a_data,
			const equipmentOverrideCondition_t& a_match)
		{
			auto form = match_pm_equipped(a_data.actor, a_match.form.get_id());
			if (!form)
			{
				auto it = a_data.forms.find(a_match.form.get_id());
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

		bool configBase_t::match_carried_form(
			const collectorData_t&              a_data,
			const equipmentOverrideCondition_t& a_match)
		{
			auto it = a_data.forms.find(a_match.form.get_id());
			if (it == a_data.forms.end())
			{
				return false;
			}

			if (it->second.count < 1)
			{
				return false;
			}

			if (a_match.keyword.get_id())
			{
				return a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch1) !=
				       has_keyword(a_match.keyword, it->second.form);
			}
			else
			{
				return true;
			}
		}

		constexpr bool configBase_t::match(
			const collectorData_t&              a_data,
			const equipmentOverrideCondition_t& a_match,
			CommonParams&                       a_params)
		{
			switch (a_match.fbf.type)
			{
			case EquipmentOverrideConditionType::Type:

				if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchAll))
				{
					return match_carried_type(a_data, a_match);
				}
				else if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
				{
					return match_equipped_type(a_data, a_match);
				}

				break;

			case EquipmentOverrideConditionType::Keyword:

				if (!a_match.keyword.get_id())
				{
					return false;
				}

				if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchAll))
				{
					return has_keyword_carried(a_match.keyword, a_data);
				}
				else if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
				{
					return has_keyword_equipped(a_match.keyword, a_data);
				}

				break;

			case EquipmentOverrideConditionType::Form:

				if (!a_match.form.get_id())
				{
					return false;
				}

				if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchAll))
				{
					return match_carried_form(a_data, a_match);
				}
				else if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
				{
					return match_equipped_form(a_data, a_match);
				}

				break;

			case EquipmentOverrideConditionType::Race:

				return Conditions::match_race<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Actor:

				return Conditions::match_form(a_match.form.get_id(), a_params.actor);

			case EquipmentOverrideConditionType::NPC:

				return Conditions::match_form(a_match.form.get_id(), a_params.npc);

			case EquipmentOverrideConditionType::Furniture:

				return Conditions::match_furniture<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::BipedSlot:

				return Conditions::match_biped<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					[](TESForm*) {});

			case EquipmentOverrideConditionType::Quest:

				return match_quest(a_params, a_match);

			case EquipmentOverrideConditionType::Extra:

				return Conditions::match_extra<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Location:

				return Conditions::match_location<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Worldspace:

				return Conditions::match_worldspace<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Package:

				return Conditions::match_package<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);
			}

			return false;
		}

		constexpr bool configBase_t::match(
			const collectorData_t&                  a_data,
			const equipmentOverrideConditionList_t& a_matches,
			CommonParams&                           a_params,
			bool                                    a_default)
		{
			bool result = a_default;

			for (auto& f : a_matches)
			{
				if (f.fbf.type == EquipmentOverrideConditionType::Group)
				{
					result = match(a_data, f.group.conditions, a_params, a_default);
				}
				else
				{
					result = match(a_data, f, a_params);
				}

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
			const collectorData_t&              a_cdata,
			const slot_container_type&          a_data,
			const equipmentOverrideCondition_t& a_match,
			CommonParams&                       a_params)
		{
			switch (a_match.fbf.type)
			{
			case EquipmentOverrideConditionType::Type:
				{
					if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchAll))
					{
						return match_carried_type(a_cdata, a_match);
					}

					std::uint32_t result = 0;
					std::uint32_t min    = a_match.flags.test(EquipmentOverrideConditionFlags::kMatchAll) &&
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

						if (a_match.form.get_id())
						{
							if (a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch1) ==
							    (form->formID == a_match.form.get_id()))
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
					if (!a_match.keyword.get_id())
					{
						return false;
					}

					if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchAll))
					{
						return has_keyword_carried(a_match.keyword, a_cdata);
					}

					std::uint32_t result = 0;
					std::uint32_t min    = a_match.flags.test(EquipmentOverrideConditionFlags::kMatchAll) &&
                                                !a_match.flags.test(EquipmentOverrideConditionFlags::kMatchCategoryOperOR) ?
					                           2u :
                                               1u;

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
					{
						result += has_keyword_equipped(a_match.keyword, a_cdata);

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
					if (!a_match.form.get_id())
					{
						return false;
					}

					if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchAll))
					{
						return match_carried_form(a_cdata, a_match);
					}

					std::uint32_t result = 0;
					std::uint32_t min    = a_match.flags.test(EquipmentOverrideConditionFlags::kMatchAll) &&
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

				return Conditions::match_race<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Actor:

				return Conditions::match_form(a_match.form.get_id(), a_params.actor);

			case EquipmentOverrideConditionType::NPC:

				return Conditions::match_form(a_match.form.get_id(), a_params.npc);

			case EquipmentOverrideConditionType::Furniture:

				return Conditions::match_furniture<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::BipedSlot:

				return Conditions::match_biped<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					[](TESForm*) {});

			case EquipmentOverrideConditionType::Quest:

				return match_quest(a_params, a_match);

			case EquipmentOverrideConditionType::Extra:

				return Conditions::match_extra<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Location:

				return Conditions::match_location<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Worldspace:

				return Conditions::match_worldspace<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Package:

				return Conditions::match_package<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);
			}

			return false;
		}

		constexpr bool configBase_t::match(
			const collectorData_t&                  a_cdata,
			const slot_container_type&              a_data,
			const equipmentOverrideConditionList_t& a_matches,
			CommonParams&                           a_params,
			bool                                    a_default)
		{
			bool result = a_default;

			for (auto& f : a_matches)
			{
				if (f.fbf.type == EquipmentOverrideConditionType::Group)
				{
					result = match(a_cdata, a_data, f.group.conditions, a_params, a_default);
				}
				else
				{
					result = match(a_cdata, a_data, f, a_params);
				}

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
			const collectorData_t&              a_data,
			const equipmentOverrideCondition_t& a_match,
			const formSlotPair_t&               a_checkForm,
			CommonParams&                       a_params)
		{
			switch (a_match.fbf.type)
			{
			case EquipmentOverrideConditionType::Type:
				{
					if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchAll))
					{
						return match_carried_type(a_data, a_match);
					}

					std::uint32_t result = 0;
					std::uint32_t min    = a_match.flags.test(EquipmentOverrideConditionFlags::kMatchAll) &&
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

						if (a_match.form.get_id())
						{
							if (a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch1) ==
							    (a_match.form.get_id() == a_checkForm.form->formID))
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
					if (!a_match.keyword.get_id())
					{
						return false;
					}

					if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchAll))
					{
						return has_keyword_carried(a_match.keyword, a_data);
					}

					std::uint32_t result = 0;
					std::uint32_t min    = a_match.flags.test(EquipmentOverrideConditionFlags::kMatchAll) &&
                                                !a_match.flags.test(EquipmentOverrideConditionFlags::kMatchCategoryOperOR) ?
					                           2u :
                                               1u;

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
					{
						result += has_keyword_equipped(a_match.keyword, a_data);

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
					if (!a_match.form.get_id())
					{
						return false;
					}

					if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchAll))
					{
						return match_carried_form(a_data, a_match);
					}

					std::uint32_t result = 0;
					std::uint32_t min    = a_match.flags.test(EquipmentOverrideConditionFlags::kMatchAll) &&
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
						if (a_match.form.get_id() != a_checkForm.form->formID)
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

				return Conditions::match_race<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Actor:

				return Conditions::match_form(a_match.form.get_id(), a_params.actor);

			case EquipmentOverrideConditionType::NPC:

				return Conditions::match_form(a_match.form.get_id(), a_params.npc);

			case EquipmentOverrideConditionType::Furniture:

				return Conditions::match_furniture<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::BipedSlot:

				return Conditions::match_biped<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					[](TESForm*) {});

			case EquipmentOverrideConditionType::Quest:

				return match_quest(a_params, a_match);

			case EquipmentOverrideConditionType::Extra:

				return Conditions::match_extra<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Location:

				return Conditions::match_location<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Worldspace:

				return Conditions::match_worldspace<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Package:

				return Conditions::match_package<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);
			}

			return false;
		}

		constexpr bool configBase_t::match(
			const collectorData_t&                  a_data,
			const equipmentOverrideConditionList_t& a_matches,
			const formSlotPair_t&                   a_checkForm,
			CommonParams&                           a_params,
			bool                                    a_default)
		{
			bool result = a_default;

			for (auto& f : a_matches)
			{
				if (f.fbf.type == EquipmentOverrideConditionType::Group)
				{
					result = match(a_data, f.group.conditions, a_checkForm, a_params, a_default);
				}
				else
				{
					result = match(a_data, f, a_checkForm, a_params);
				}

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

		bool configBase_t::has_keyword_equipped(
			const configCachedForm_t& a_keyword,
			const collectorData_t&    a_data)
		{
			if (auto form = a_keyword.get_form())
			{
				if (auto keyword = form->As<BGSKeyword>())
				{
					if (auto pm = a_data.actor->processManager)
					{
						for (auto& e : pm->equippedObject)
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

		bool configBase_t::has_keyword_carried(
			const configCachedForm_t& a_keyword,
			const collectorData_t&    a_data)
		{
			if (auto form = a_keyword.get_form())
			{
				if (auto keyword = form->As<BGSKeyword>())
				{
					for (auto& e : a_data.forms)
					{
						if (e.second.count > 0)
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
			const configCachedForm_t&  a_keyword,
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
			TESForm*                  a_form)
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

		bool configBase_t::has_keyword_carried(
			const configCachedForm_t& a_keyword,
			ObjectTypeExtra           a_type,
			const collectorData_t&    a_data)
		{
			if (auto form = a_keyword.get_form())
			{
				if (auto keyword = form->As<BGSKeyword>())
				{
					for (auto& e : a_data.forms)
					{
						if (e.second.count > 0 && e.second.typeExtra == a_type)
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

		bool configBase_t::has_keyword_equipped(
			const configCachedForm_t& a_keyword,
			ObjectSlotExtra           a_slot,
			const collectorData_t&    a_data)
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
			const configCachedForm_t&  a_keyword,
			ObjectSlot                 a_slot,
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
			const slot_container_type&          a_data,
			const equipmentOverrideCondition_t& a_match)
		{
			for (auto& e : a_data)
			{
				if (auto form = e.GetFormIfActive())
				{
					if (form->formID == a_match.form.get_id())
					{
						return form;
					}
				}
			}

			return nullptr;
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