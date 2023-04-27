#include "pch.h"

#include "ConditionsCommon.h"
#include "ConfigBase.h"
#include "FormCommon.h"

#include "Controller/ObjectManagerData.h"

#include "ProcessParams.h"

namespace IED
{
	formSlotPair_t::formSlotPair_t(
		TESForm* a_form) noexcept :
		form(a_form)
	{
		const auto exslot = Data::ItemData::GetItemSlotExtraGeneric(a_form);

		slot  = exslot;
		slot2 = Data::ItemData::ExtraSlotToSlot(exslot);
	}

	formSlotPair_t::formSlotPair_t(
		TESForm*              a_form,
		Data::ObjectSlotExtra a_slot) noexcept :
		form(a_form),
		slot(a_slot),
		slot2(Data::ItemData::ExtraSlotToSlot(a_slot))
	{
	}

	namespace Data
	{
		const equipmentOverride_t* configBase_t::get_equipment_override(
			ProcessParams& a_params) const noexcept
		{
			return get_equipment_override(
				a_params,
				equipmentOverrides);
		}

		const equipmentOverride_t* configBase_t::get_equipment_override(
			ProcessParams&                 a_params,
			const equipmentOverrideList_t& a_list) noexcept
		{
			for (auto& e : a_list)
			{
				if (do_match(e.conditions, a_params, true))
				{
					if (e.overrideFlags.test(Data::ConfigOverrideFlags::kIsGroup))
					{
						if (auto result = get_equipment_override(
								a_params,
								e.group))
						{
							return result;
						}

						if (!e.overrideFlags.test(Data::ConfigOverrideFlags::kContinue))
						{
							break;
						}
					}
					else
					{
						return std::addressof(e);
					}
				}
			}

			return nullptr;
		}

		const equipmentOverride_t* configBase_t::get_equipment_override_fp(
			const formSlotPair_t& a_checkForm,
			ProcessParams&        a_params) const noexcept
		{
			return get_equipment_override_fp(
				a_checkForm,
				a_params,
				equipmentOverrides);
		}

		const equipmentOverride_t* configBase_t::get_equipment_override_fp(
			const formSlotPair_t&          a_checkForm,
			ProcessParams&                 a_params,
			const equipmentOverrideList_t& a_list) noexcept
		{
			for (auto& e : a_list)
			{
				if (do_match_fp(e.conditions, a_checkForm, a_params, true))
				{
					if (e.overrideFlags.test(Data::ConfigOverrideFlags::kIsGroup))
					{
						if (auto result = get_equipment_override_fp(
								a_checkForm,
								a_params,
								e.group))
						{
							return result;
						}

						if (!e.overrideFlags.test(Data::ConfigOverrideFlags::kContinue))
						{
							break;
						}
					}
					else
					{
						return std::addressof(e);
					}
				}
			}

			return nullptr;
		}

		const equipmentOverride_t* configBase_t::get_equipment_override_sfp(
			const formSlotPair_t& a_checkForm,
			ProcessParams&        a_params) const noexcept
		{
			return get_equipment_override_sfp(
				a_checkForm,
				a_params,
				equipmentOverrides);
		}

		const equipmentOverride_t* configBase_t::get_equipment_override_sfp(
			const formSlotPair_t&          a_checkForm,
			ProcessParams&                 a_params,
			const equipmentOverrideList_t& a_list) noexcept
		{
			for (auto& e : a_list)
			{
				if (do_match_sfp(e.conditions, a_checkForm, a_params, true))
				{
					if (e.overrideFlags.test(Data::ConfigOverrideFlags::kIsGroup))
					{
						if (auto result = get_equipment_override_sfp(
								a_checkForm,
								a_params,
								e.group))
						{
							return result;
						}

						if (!e.overrideFlags.test(Data::ConfigOverrideFlags::kContinue))
						{
							break;
						}
					}
					else
					{
						return std::addressof(e);
					}
				}
			}

			return nullptr;
		}

		const configEffectShaderHolder_t* configBase_t::get_effect_shader(
			ProcessParams& a_params) const noexcept
		{
			for (auto& e : effectShaders.data)
			{
				if (!e.enabled())
				{
					continue;
				}

				if (do_match(e.conditions, a_params, true))
				{
					return std::addressof(e);
				}
			}

			return nullptr;
		}

		const configEffectShaderHolder_t* configBase_t::get_effect_shader_fp(
			const formSlotPair_t& a_checkForm,
			ProcessParams&        a_params) const noexcept
		{
			for (auto& e : effectShaders.data)
			{
				if (!e.enabled())
				{
					continue;
				}

				if (do_match_fp(e.conditions, a_checkForm, a_params, true))
				{
					return std::addressof(e);
				}
			}

			return nullptr;
		}

		const configEffectShaderHolder_t* configBase_t::get_effect_shader_sfp(
			const formSlotPair_t& a_checkForm,
			ProcessParams&        a_params) const noexcept
		{
			for (auto& e : effectShaders.data)
			{
				if (!e.enabled())
				{
					continue;
				}

				if (do_match_sfp(e.conditions, a_checkForm, a_params, true))
				{
					return std::addressof(e);
				}
			}

			return nullptr;
		}

		static bool match_quest(
			CommonParams&                       a_params,
			const equipmentOverrideCondition_t& a_match) noexcept
		{
			auto form = a_match.keyword.get_form<TESQuest>();
			if (!form)
			{
				return false;
			}

			switch (a_match.questCondType)
			{
			case Data::QuestConditionType::kComplete:
				return form->IsCompleted();
			case Data::QuestConditionType::kRunning:
				return !form->IsStopped() && form->IsRunning();
			case Data::QuestConditionType::kEnabled:
				return form->IsEnabled();
			default:
				return false;
			}
		}

		static TESForm* match_slot_form(
			const ObjectSlotArray& a_data,
			Game::FormID           a_formid) noexcept
		{
			for (auto& e : a_data)
			{
				if (auto form = e.GetFormIfActive())
				{
					if (form->formID == a_formid)
					{
						return form;
					}
				}
			}

			return nullptr;
		}

		bool configBase_t::match_equipped_type(
			ProcessParams&                      a_params,
			const equipmentOverrideCondition_t& a_match) noexcept
		{
			auto slot = stl::underlying(a_match.slot);
			if (slot >= stl::underlying(ObjectSlotExtra::kMax))
			{
				return false;
			}

			if (Conditions::is_hand_slot(a_match.slot))
			{
				if (a_match.flags.test(EquipmentOverrideConditionFlags::kExtraFlag1))
				{
					return a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch3);
				}

				auto pm = a_params.actor->processManager;
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
				const auto& data = a_params.collector.data;

				if (!data.IsSlotEquipped(slot))
				{
					return false;
				}

				if (a_match.flags.test(EquipmentOverrideConditionFlags::kExtraFlag1))
				{
					const auto rv = a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch3);

					if (a_match.slot != ObjectSlotExtra::kAmmo)
					{
						return rv;
					}

					const auto* const biped = a_params.get_biped();
					if (!biped)
					{
						return rv;
					}

					auto& e = biped->get_object(BIPED_OBJECT::kQuiver);

					const auto* const form = e.item;
					if (!form || e.addon == form)
					{
						return rv;
					}

					if (Conditions::is_ammo_bolt(form) == rv)
					{
						return false;
					}
				}

				if (const auto fid = a_match.form.get_id())
				{
					const auto rv = a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch1);

					auto it = data.forms.find(fid);
					if (it == data.forms.end())
					{
						return rv;
					}

					if (it->second.extra.equipped.slot != a_match.slot &&
					    it->second.extra.equipped.slotLeft != a_match.slot)
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
						    type_has_keyword_equipped(a_match, data))
						{
							return false;
						}
					}

					return true;
				}
			}
		}

		bool configBase_t::match_carried_type(
			const CollectorData&                a_data,
			const equipmentOverrideCondition_t& a_match) noexcept
		{
			const auto type = Data::ItemData::GetTypeFromSlotExtra(a_match.slot);

			if (type >= ObjectTypeExtra::kMax)
			{
				return false;
			}

			if (!a_data.IsTypePresent(type))
			{
				return false;
			}

			if (const auto fid = a_match.form.get_id())
			{
				auto rv = a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch1);

				auto it = a_data.forms.find(fid);
				if (it == a_data.forms.end())
				{
					return rv;
				}

				if (it->second.itemCount <= 0 ||
				    it->second.extra.typeExtra != type)
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
		}

		bool configBase_t::match_equipped_form(
			ProcessParams&                      a_params,
			const equipmentOverrideCondition_t& a_match) noexcept
		{
			const auto fid = a_match.form.get_id();

			const auto* form = Conditions::match_pm_equipped(a_params.actor, fid);
			if (!form)
			{
				auto it = a_params.collector.data.forms.find(fid);
				if (it == a_params.collector.data.forms.end())
				{
					return false;
				}

				form = it->second.form;

				if (!it->second.is_equipped())
				{
					return false;
				}
			}

			if (a_match.keyword.get_id())
			{
				return a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch1) !=
				       IFormCommon::HasKeyword(form, a_match.keyword);
			}
			else
			{
				return true;
			}
		}

		bool configBase_t::match_carried_form(
			const CollectorData&                a_data,
			const equipmentOverrideCondition_t& a_match) noexcept
		{
			auto it = a_data.forms.find(a_match.form.get_id());

			const auto itemCount = it == a_data.forms.end() ? 0 : it->second.itemCount;

			if (a_match.flags.test(EquipmentOverrideConditionFlags::kExtraFlag1))
			{
				if (!Conditions::compare(a_match.compOperator, static_cast<std::int64_t>(itemCount), a_match.count))
				{
					return false;
				}
			}
			else
			{
				if (itemCount <= 0)
				{
					return false;
				}
			}

			if (a_match.keyword.get_id())
			{
				if (it == a_data.forms.end())
				{
					return false;
				}

				return a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch1) !=
				       IFormCommon::HasKeyword(it->second.form, a_match.keyword);
			}
			else
			{
				return true;
			}
		}

		bool configBase_t::match_equipped(
			const equipmentOverrideCondition_t& a_match,
			ProcessParams&                      a_params) noexcept
		{
			switch (a_match.fbf.type)
			{
			case EquipmentOverrideConditionType::Type:

				if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchMaskAny))
				{
					return match_carried_type(a_params.collector.data, a_match);
				}
				else if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
				{
					return match_equipped_type(a_params, a_match);
				}

				break;

			case EquipmentOverrideConditionType::Keyword:

				if (!a_match.keyword.get_id())
				{
					return false;
				}

				if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchMaskAny))
				{
					return has_keyword_carried(a_match.keyword, a_params.collector.data);
				}
				else if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
				{
					return has_keyword_equipped(a_match.keyword, a_params);
				}

				break;

			case EquipmentOverrideConditionType::Form:

				if (!a_match.form.get_id())
				{
					return false;
				}

				if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchMaskAny))
				{
					return match_carried_form(a_params.collector.data, a_match);
				}
				else if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
				{
					return match_equipped_form(a_params, a_match);
				}

				break;

			case EquipmentOverrideConditionType::Race:

				return Conditions::match_race<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Actor:

				return Conditions::match_actor<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::NPC:

				return Conditions::match_npc<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

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
					[](auto*) {});

			case EquipmentOverrideConditionType::Quest:

				return match_quest(a_params, a_match);

			case EquipmentOverrideConditionType::Extra:

				return Conditions::match_extra<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Location:

				return Conditions::match_location<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Worldspace:

				return Conditions::match_worldspace<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Package:

				return Conditions::match_package<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Weather:

				return Conditions::match_weather<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Global:

				return Conditions::match_global<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Mounting:

				return Conditions::match_mount<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Mounted:

				return Conditions::match_mounted_by<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Idle:

				return Conditions::match_idle<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Skeleton:

				return Conditions::match_skeleton<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match);

			case EquipmentOverrideConditionType::Faction:

				return Conditions::match_faction<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Effect:

				return Conditions::match_effect<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Variable:

				if (!a_params.flags.test(ControllerUpdateFlags::kFailVariableCondition))
				{
					return Conditions::match_variable<
						equipmentOverrideCondition_t,
						EquipmentOverrideConditionFlags>(
						a_params,
						a_match);
				}

				break;

			case EquipmentOverrideConditionType::Perk:

				return Conditions::match_perk<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Cell:

				return Conditions::match_cell<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match);
			}

			return false;
		}

		bool configBase_t::do_match(
			const equipmentOverrideConditionList_t& a_matches,
			ProcessParams&                          a_params,
			bool                                    a_default) noexcept
		{
			bool result = a_default;

			for (auto& f : a_matches)
			{
				if (f.fbf.type == EquipmentOverrideConditionType::Group)
				{
					result = do_match(f.group.conditions, a_params, a_default);
				}
				else
				{
					result = match_equipped(f, a_params);
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

		bool configBase_t::match_equipped_or_slot(
			const equipmentOverrideCondition_t& a_match,
			ProcessParams&                      a_params) noexcept
		{
			switch (a_match.fbf.type)
			{
			case EquipmentOverrideConditionType::Type:
				{
					if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchMaskEquippedAndSlots))
					{
						return match_carried_type(a_params.collector.data, a_match);
					}

					std::uint32_t result = 0;
					std::uint32_t min    = a_match.flags.test(EquipmentOverrideConditionFlags::kMatchMaskEquippedAndSlots) &&
                                                !a_match.flags.test(EquipmentOverrideConditionFlags::kMatchCategoryOperOR) ?
					                           2u :
					                           1u;

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
					{
						result += match_equipped_type(a_params, a_match);

						if (result == min)
						{
							return true;
						}
					}

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipmentSlots))
					{
						auto sid = ItemData::ExtraSlotToSlot(a_match.slot);
						if (sid >= ObjectSlot::kMax)
						{
							return false;
						}

						auto form = a_params.objects.GetSlot(sid).GetFormIfActive();
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

						if (a_match.flags.test(EquipmentOverrideConditionFlags::kExtraFlag1))
						{
							if (a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch3) ==
							    (sid == ObjectSlot::kAmmo && Conditions::is_ammo_bolt(form)))
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

					if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchMaskEquippedAndSlots))
					{
						return has_keyword_carried(a_match.keyword, a_params.collector.data);
					}

					std::uint32_t result = 0;
					std::uint32_t min    = a_match.flags.test(EquipmentOverrideConditionFlags::kMatchMaskEquippedAndSlots) &&
                                                !a_match.flags.test(EquipmentOverrideConditionFlags::kMatchCategoryOperOR) ?
					                           2u :
					                           1u;

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
					{
						result += has_keyword_equipped(a_match.keyword, a_params);

						if (result == min)
						{
							return true;
						}
					}

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipmentSlots))
					{
						result += has_keyword_slot(a_match.keyword, a_params);
					}

					return result == min;
				}
			case EquipmentOverrideConditionType::Form:
				{
					if (!a_match.form.get_id())
					{
						return false;
					}

					if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchMaskEquippedAndSlots))
					{
						return match_carried_form(a_params.collector.data, a_match);
					}

					std::uint32_t result = 0;
					std::uint32_t min    = a_match.flags.test(EquipmentOverrideConditionFlags::kMatchMaskEquippedAndSlots) &&
                                                !a_match.flags.test(EquipmentOverrideConditionFlags::kMatchCategoryOperOR) ?
					                           2u :
					                           1u;

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
					{
						result += match_equipped_form(a_params, a_match);

						if (result == min)
						{
							return true;
						}
					}

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipmentSlots))
					{
						auto form = match_slot_form(a_params.objects.GetSlots(), a_match.form.get_id());
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

				return Conditions::match_actor<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::NPC:

				return Conditions::match_npc<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

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
					[](auto*) {});

			case EquipmentOverrideConditionType::Quest:

				return match_quest(a_params, a_match);

			case EquipmentOverrideConditionType::Extra:

				return Conditions::match_extra<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Location:

				return Conditions::match_location<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Worldspace:

				return Conditions::match_worldspace<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Package:

				return Conditions::match_package<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Weather:

				return Conditions::match_weather<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Global:

				return Conditions::match_global<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Mounting:

				return Conditions::match_mount<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Mounted:

				return Conditions::match_mounted_by<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Idle:

				return Conditions::match_idle<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Skeleton:

				return Conditions::match_skeleton<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match);

			case EquipmentOverrideConditionType::Faction:

				return Conditions::match_faction<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Effect:

				return Conditions::match_effect<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Variable:

				if (!a_params.flags.test(ControllerUpdateFlags::kFailVariableCondition))
				{
					return Conditions::match_variable<
						equipmentOverrideCondition_t,
						EquipmentOverrideConditionFlags>(
						a_params,
						a_match);
				}

				break;

			case EquipmentOverrideConditionType::Perk:

				return Conditions::match_perk<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Cell:

				return Conditions::match_cell<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match);
			}

			return false;
		}

		static bool match_presence_equipped(
			const CollectorData&                a_data,
			const equipmentOverrideCondition_t& a_match,
			const formSlotPair_t&               a_checkForm,
			CommonParams&                       a_params) noexcept
		{
			if (a_match.bipedSlot == BIPED_OBJECT::kNone)
			{
				if (auto pm = a_params.actor->processManager)
				{
					for (auto e : pm->equippedObject)
					{
						if (e == a_checkForm.form)
						{
							return true;
						}
					}
				}

				auto it = a_data.forms.find(a_checkForm.form->formID);
				if (it == a_data.forms.end())
				{
					return false;
				}

				switch (a_match.fbf.presenceEquipedHandMatch)
				{
				case PresenceEquippedHandMatch::kLeft:
					return it->second.is_equipped_left();
				case PresenceEquippedHandMatch::kRight:
					return it->second.is_equipped_right();
				default:
					return it->second.is_equipped();
				}
			}
			else
			{
				const auto slot = a_params.translate_biped_object(a_match.bipedSlot);

				if (slot >= BIPED_OBJECT::kTotal)
				{
					return false;
				}

				auto biped = a_params.get_biped();
				if (!biped)
				{
					return false;
				}

				auto& e = biped->get_object(slot);

				auto form = e.item;
				if (!form || e.addon == form)
				{
					return false;
				}

				if (a_match.flags.test(EquipmentOverrideConditionFlags::kExtraFlag8))
				{
					if (!e.object || !e.object->IsVisible())
					{
						return false;
					}
				}

				return form == a_checkForm.form;
			}
		}

		static bool match_presence_slots(
			const equipmentOverrideCondition_t& a_match,
			const formSlotPair_t&               a_checkForm,
			CommonParams&                       a_params) noexcept
		{
			if (a_match.slot == Data::ObjectSlotExtra::kNone)
			{
				auto& slots = a_params.objects.GetSlots();

				auto it = std::find_if(
					slots.begin(),
					slots.end(),
					[&](auto& a_e) noexcept [[msvc::forceinline]] {
						return a_e.GetFormIfActive() == a_checkForm.form;
					});

				return it != slots.end();
			}
			else
			{
				const auto sid = ItemData::ExtraSlotToSlot(a_match.slot);
				if (sid >= ObjectSlot::kMax)
				{
					return false;
				}

				const auto& slot = a_params.objects.GetSlot(sid);

				return slot.GetFormIfActive() == a_checkForm.form;
			}
		}

		static bool match_presence_count(
			const CollectorData&                a_data,
			const equipmentOverrideCondition_t& a_match,
			const formSlotPair_t&               a_checkForm) noexcept
		{
			auto it = a_data.forms.find(a_checkForm.form->formID);
			if (it == a_data.forms.end())
			{
				return false;
			}

			if (a_match.flags.test(EquipmentOverrideConditionFlags::kExtraFlag1))
			{
				return Conditions::compare(a_match.compOperator, static_cast<std::int64_t>(it->second.itemCount), a_match.count);
			}
			else
			{
				return it->second.itemCount > 0;
			}
		}

		static bool match_presence_available(
			const ProcessParams&                a_params,
			const equipmentOverrideCondition_t& a_match,
			const formSlotPair_t&               a_checkForm) noexcept
		{
			const auto& fdata = a_params.collector.data;

			auto form = a_checkForm.form;

			auto it = fdata.forms.find(form->formID);
			if (it == fdata.forms.end())
			{
				return false;
			}

			std::int64_t count = it->second.itemCount;

			const auto slot = a_checkForm.slot2;

			if (it->second.is_equipped_right())
			{
				count--;
			}
			else if (slot < ObjectSlot::kMax)
			{
				if (a_params.objects.GetSlot(slot).GetFormIfActive() == form)
				{
					count--;
				}
			}

			if (it->second.is_equipped_left())
			{
				count--;
			}
			else if (const auto leftSlot = ItemData::GetLeftSlot(slot); leftSlot < ObjectSlot::kMax)
			{
				if (a_params.objects.GetSlot(leftSlot).GetFormIfActive() == form)
				{
					count--;
				}
			}

			auto itc = a_params.useCount.find(form->formID);
			if (itc != a_params.useCount.end())
			{
				count -= itc->second;
			}

			return count > 0;
		}

		static constexpr bool match_equip_slot(
			Game::FormID a_matchForm,
			TESForm*     a_form) noexcept
		{
			BGSEquipSlot* slot;

			switch (a_form->formType)
			{
			case SpellItem::kTypeID:
				slot = static_cast<const SpellItem*>(a_form)->equipSlot;
				break;
			case ScrollItem::kTypeID:
				slot = static_cast<const ScrollItem*>(a_form)->equipSlot;
				break;
			case TESObjectWEAP::kTypeID:
				slot = static_cast<const TESObjectWEAP*>(a_form)->equipSlot;
				break;
			default:
				return false;
			}

			return slot && slot->formID == a_matchForm;
		}

		static constexpr bool match_is_favorited(
			const ProcessParams& a_params,
			Game::FormID         a_formid) noexcept
		{
			const auto& data = a_params.collector.data.forms;

			auto it = data.find(a_formid);
			if (it != data.end())
			{
				return it->second.is_favorited();
			}
			else
			{
				return false;
			}
		}

		template <
			EquipmentOverrideConditionFlags a_maskAll,
			EquipmentOverrideConditionFlags a_maskSlots>
		bool configBase_t::match_equipped_or_form(
			const equipmentOverrideCondition_t& a_match,
			const formSlotPair_t&               a_checkForm,
			ProcessParams&                      a_params) noexcept
		{
			switch (a_match.fbf.type)
			{
			case EquipmentOverrideConditionType::Type:
				{
					if (!a_match.flags.test_any(a_maskAll))
					{
						return match_carried_type(a_params.collector.data, a_match);
					}

					std::uint32_t result = 0;
					std::uint32_t min    = a_match.flags.test(a_maskAll) &&
                                                !a_match.flags.test(EquipmentOverrideConditionFlags::kMatchCategoryOperOR) ?
					                           2u :
					                           1u;

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
					{
						result += match_equipped_type(a_params, a_match);

						if (result == min)
						{
							return true;
						}
					}

					if (a_match.flags.test_any(a_maskSlots))
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
							    IFormCommon::HasKeyword(a_checkForm.form, a_match.keyword))
							{
								return false;
							}
						}

						if (a_match.flags.test(EquipmentOverrideConditionFlags::kExtraFlag1))
						{
							if (a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch3) ==
							    (a_match.slot == ObjectSlotExtra::kAmmo && Conditions::is_ammo_bolt(a_checkForm.form)))
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

					if (!a_match.flags.test_any(a_maskAll))
					{
						return has_keyword_carried(a_match.keyword, a_params.collector.data);
					}

					std::uint32_t result = 0;
					std::uint32_t min    = a_match.flags.test(a_maskAll) &&
                                                !a_match.flags.test(EquipmentOverrideConditionFlags::kMatchCategoryOperOR) ?
					                           2u :
					                           1u;

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
					{
						result += has_keyword_equipped(a_match.keyword, a_params);

						if (result == min)
						{
							return true;
						}
					}

					if (a_match.flags.test_any(a_maskSlots))
					{
						result += IFormCommon::HasKeyword(a_checkForm.form, a_match.keyword);
					}

					return result == min;
				}
			case EquipmentOverrideConditionType::Form:
				{
					if (!a_match.form.get_id())
					{
						return false;
					}

					if (!a_match.flags.test_any(a_maskAll))
					{
						return match_carried_form(a_params.collector.data, a_match);
					}

					std::uint32_t result = 0;
					std::uint32_t min    = a_match.flags.test(a_maskAll) &&
                                                !a_match.flags.test(EquipmentOverrideConditionFlags::kMatchCategoryOperOR) ?
					                           2u :
					                           1u;

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
					{
						result += match_equipped_form(a_params, a_match);

						if (result == min)
						{
							return true;
						}
					}

					if (a_match.flags.test_any(a_maskSlots))
					{
						if (a_match.form.get_id() != a_checkForm.form->formID)
						{
							return false;
						}

						if (a_match.keyword.get_id())
						{
							if (a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch1) ==
							    IFormCommon::HasKeyword(a_checkForm.form, a_match.keyword))
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

				return Conditions::match_actor<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::NPC:

				return Conditions::match_npc<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

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
					[](auto*) {});

			case EquipmentOverrideConditionType::Quest:

				return match_quest(a_params, a_match);

			case EquipmentOverrideConditionType::Extra:

				return Conditions::match_extra<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Location:

				return Conditions::match_location<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Worldspace:

				return Conditions::match_worldspace<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Package:

				return Conditions::match_package<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Weather:

				return Conditions::match_weather<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Global:

				return Conditions::match_global<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Mounting:

				return Conditions::match_mount<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(a_params, a_match);

			case EquipmentOverrideConditionType::Mounted:

				return Conditions::match_mounted_by<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Presence:
				{
					if (a_match.flags.test(EquipmentOverrideConditionFlags::kExtraFlag3))
					{
						if (!Conditions::is_ammo_bolt(a_checkForm.form))
						{
							return false;
						}
					}

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kExtraFlag4))
					{
						if (!a_checkForm.form->GetPlayable())
						{
							return false;
						}
					}

					if (auto& formid = a_match.form.get_id())
					{
						if (a_match.flags.test(EquipmentOverrideConditionFlags::kNegateMatch1) ==
						    match_equip_slot(formid, a_checkForm.form))
						{
							return false;
						}
					}

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kExtraFlag5))
					{
						if (!match_is_favorited(a_params, a_checkForm.form->formID))
						{
							return false;
						}
					}

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kExtraFlag2))
					{
						return match_presence_available(
							a_params,
							a_match,
							a_checkForm);
					}

					if (!a_match.flags.test_any(EquipmentOverrideConditionFlags::kMatchMaskEquippedAndSlots))
					{
						return match_presence_count(
							a_params.collector.data,
							a_match,
							a_checkForm);
					}

					std::uint32_t           result = 0;
					constexpr std::uint32_t min    = 1u;

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipped))
					{
						result += match_presence_equipped(
							a_params.collector.data,
							a_match,
							a_checkForm,
							a_params);

						if (result == min)
						{
							return true;
						}
					}

					if (a_match.flags.test(EquipmentOverrideConditionFlags::kMatchEquipmentSlots))
					{
						result += match_presence_slots(
							a_match,
							a_checkForm,
							a_params);
					}

					return result == min;
				}
				break;

			case EquipmentOverrideConditionType::Idle:

				return Conditions::match_idle<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Skeleton:

				return Conditions::match_skeleton<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match);

			case EquipmentOverrideConditionType::Faction:

				return Conditions::match_faction<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Effect:

				return Conditions::match_effect<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Variable:

				if (!a_params.flags.test(ControllerUpdateFlags::kFailVariableCondition))
				{
					return Conditions::match_variable<
						equipmentOverrideCondition_t,
						EquipmentOverrideConditionFlags>(
						a_params,
						a_match);
				}

				break;

			case EquipmentOverrideConditionType::Perk:

				return Conditions::match_perk<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match,
					a_params.objects.GetCachedData());

			case EquipmentOverrideConditionType::Cell:

				return Conditions::match_cell<
					equipmentOverrideCondition_t,
					EquipmentOverrideConditionFlags>(
					a_params,
					a_match);
			}

			return false;
		}

		bool configBase_t::do_match_fp(
			const equipmentOverrideConditionList_t& a_matches,
			const formSlotPair_t&                   a_checkForm,
			ProcessParams&                          a_params,
			bool                                    a_default) noexcept
		{
			bool result = a_default;

			for (auto& f : a_matches)
			{
				if (f.fbf.type == EquipmentOverrideConditionType::Group)
				{
					result = do_match_fp(f.group.conditions, a_checkForm, a_params, a_default);
				}
				else
				{
					result = match_equipped_or_form<
						EquipmentOverrideConditionFlags::kMatchMaskAny,
						EquipmentOverrideConditionFlags::kMatchMaskAllEquipmentSlotsAndThis>(
						f,
						a_checkForm,
						a_params);
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

		bool configBase_t::do_match_sfp(
			const equipmentOverrideConditionList_t& a_matches,
			const formSlotPair_t&                   a_checkForm,
			ProcessParams&                          a_params,
			bool                                    a_default) noexcept
		{
			bool result = a_default;

			for (auto& f : a_matches)
			{
				if (f.fbf.type == EquipmentOverrideConditionType::Group)
				{
					result = do_match_sfp(f.group.conditions, a_checkForm, a_params, a_default);
				}
				else
				{
					if (f.flags.test(Data::EquipmentOverrideConditionFlags::kMatchThisItem) ||
					    f.fbf.type == EquipmentOverrideConditionType::Presence)
					{
						result = match_equipped_or_form<
							EquipmentOverrideConditionFlags::kMatchMaskEquippedAndThis,
							EquipmentOverrideConditionFlags::kMatchThisItem>(
							f,
							a_checkForm,
							a_params);
					}
					else
					{
						result = match_equipped_or_slot(f, a_params);
					}
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

		bool configBase_t::do_match_eos(
			const equipmentOverrideConditionList_t& a_matches,
			ProcessParams&                          a_params,
			bool                                    a_default) noexcept
		{
			bool result = a_default;

			for (auto& f : a_matches)
			{
				if (f.fbf.type == EquipmentOverrideConditionType::Group)
				{
					result = do_match_eos(f.group.conditions, a_params, a_default);
				}
				else
				{
					result = match_equipped_or_slot(f, a_params);
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
			ProcessParams&            a_params) noexcept
		{
			if (auto keyword = a_keyword.get_form<BGSKeyword>())
			{
				if (auto pm = a_params.actor->processManager)
				{
					for (auto e : pm->equippedObject)
					{
						if (e && IFormCommon::HasKeyword(e, keyword))
						{
							return true;
						}
					}
				}

				for (auto& e : a_params.collector.data.equippedForms)
				{
					if (IFormCommon::HasKeyword(e.form, keyword))
					{
						return true;
					}
				}
			}

			return false;
		}

		bool configBase_t::has_keyword_carried(
			const configCachedForm_t& a_keyword,
			const CollectorData&      a_data) noexcept
		{
			if (auto keyword = a_keyword.get_form<BGSKeyword>())
			{
				for (auto& e : a_data.forms)
				{
					if (e.second.itemCount > 0)
					{
						if (IFormCommon::HasKeyword(e.second.form, keyword))
						{
							return true;
						}
					}
				}
			}

			return false;
		}

		bool configBase_t::has_keyword_slot(
			const configCachedForm_t& a_keyword,
			CommonParams&             a_params) noexcept
		{
			if (auto keyword = a_keyword.get_form<BGSKeyword>())
			{
				for (auto& e : a_params.objects.GetSlots())
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

			return false;
		}

		bool configBase_t::has_keyword_carried(
			const configCachedForm_t& a_keyword,
			ObjectTypeExtra           a_type,
			const CollectorData&      a_data) noexcept
		{
			if (auto keyword = a_keyword.get_form<BGSKeyword>())
			{
				for (auto& e : a_data.forms)
				{
					if (e.second.itemCount > 0 && e.second.extra.typeExtra == a_type)
					{
						if (IFormCommon::HasKeyword(e.second.form, keyword))
						{
							return true;
						}
					}
				}
			}

			return false;
		}

		bool configBase_t::type_has_keyword_equipped(
			const equipmentOverrideCondition_t& a_match,
			const CollectorData&                a_data) noexcept
		{
			if (auto keyword = a_match.keyword.get_form<BGSKeyword>())
			{
				const auto slot = a_match.slot;

				for (auto& e : a_data.equippedForms)
				{
					if (e.extraEquipped.slot == slot ||
					    e.extraEquipped.slotLeft == slot)
					{
						if (IFormCommon::HasKeyword(e.form, keyword))
						{
							return true;
						}
					}
				}
			}

			return false;
		}

		bool configBaseFiltersHolder_t::run_filters(
			const CommonParams& a_params) const noexcept
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