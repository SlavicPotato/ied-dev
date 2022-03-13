#include "pch.h"

#include "Controller.h"
#include "IED/ConfigConditionsCommon.h"
#include "IED/ConfigNodeOverride.h"
#include "IED/EngineExtensions.h"
#include "IED/FormCommon.h"
#include "IED/Inventory.h"
#include "INodeOverride.h"
#include "ObjectManagerData.h"

namespace IED
{
	static bool match_form_slot(
		const Data::configNodeOverrideCondition_t& a_data,
		const INodeOverride::nodeOverrideParams_t& a_params)
	{
		TESForm* foundForm = nullptr;

		for (auto& e : a_params.objects.GetSlots())
		{
			if (auto form = e.GetFormIfActive())
			{
				if (form->formID == a_data.form.get_id())
				{
					foundForm = form;
					break;
				}
			}
		}

		if (!foundForm)
		{
			return false;
		}

		if (a_data.keyword.get_id())
		{
			if (a_data.flags.test(Data::NodeOverrideConditionFlags::kNegateMatch1) ==
			    IFormCommon::HasKeyword(foundForm, a_data.keyword))
			{
				return false;
			}
		}

		return true;
	}

	static constexpr bool match_keyword_slot(
		BGSKeyword*                                a_keyword,
		const INodeOverride::nodeOverrideParams_t& a_params)
	{
		for (auto& e : a_params.objects.GetSlots())
		{
			if (auto form = e.GetFormIfActive())
			{
				if (IFormCommon::HasKeyword(form, a_keyword))
				{
					return true;
				}
			}
		}

		return false;
	}

	static bool match_keyword_equipped(
		BGSKeyword*                          a_keyword,
		INodeOverride::nodeOverrideParams_t& a_params)
	{
		if (auto pm = a_params.actor->processManager)
		{
			for (auto e : pm->equippedObject)
			{
				if (e && IFormCommon::HasKeyword(e, a_keyword))
				{
					return true;
				}
			}
		}

		auto data = a_params.get_item_data();

		for (auto& e : *data)
		{
			if (IFormCommon::HasKeyword(e.second.item, a_keyword))
			{
				if (e.second.item->IsArmor())
				{
					e.second.matched = true;
				}

				return true;
			}
		}

		return false;
	}

	static TESForm* find_equipped_form_pm(
		Game::FormID                               a_formid,
		const INodeOverride::nodeOverrideParams_t& a_params)
	{
		if (auto pm = a_params.actor->processManager)
		{
			for (auto e : pm->equippedObject)
			{
				if (e && e->formID == a_formid)
				{
					return e;
				}
			}
		}

		return nullptr;
	}

	static auto find_equipped_form(
		Game::FormID                         a_form,
		INodeOverride::nodeOverrideParams_t& a_params)
		-> std::pair<TESForm*, INodeOverride::nodeOverrideParams_t::item_container_type::value_type*>
	{
		if (auto form = find_equipped_form_pm(a_form, a_params))
		{
			return { form, nullptr };
		}

		auto data = a_params.get_item_data();

		auto it = data->find(a_form);
		if (it != data->end())
		{
			return { it->second.item, std::addressof(*it) };
		}

		return { nullptr, nullptr };
	}

	static bool match_form_equipped(
		const Data::configNodeOverrideCondition_t& a_data,
		INodeOverride::nodeOverrideParams_t&       a_params)
	{
		auto r = find_equipped_form(a_data.form.get_id(), a_params);

		if (!r.first)
		{
			return false;
		}

		if (a_data.keyword.get_id())
		{
			if (a_data.flags.test(Data::NodeOverrideConditionFlags::kNegateMatch1) ==
			    IFormCommon::HasKeyword(r.first, a_data.keyword))
			{
				return false;
			}
		}

		if (r.second && r.second->second.item->IsArmor())
		{
			r.second->second.matched = true;
		}

		return true;
	}

	constexpr bool match_slotted_type(
		const Data::configNodeOverrideCondition_t& a_match,
		INodeOverride::nodeOverrideParams_t&       a_params)
	{
		auto slot = stl::underlying(Data::ItemData::ExtraSlotToSlot(a_match.typeSlot));
		if (slot >= stl::underlying(Data::ObjectSlot::kMax))
		{
			return false;
		}

		auto& slots = a_params.objects.GetSlots();

		auto form = slots[slot].GetFormIfActive();
		if (!form)
		{
			return false;
		}

		if (a_match.form.get_id())
		{
			if (a_match.flags.test(Data::NodeOverrideConditionFlags::kNegateMatch1) ==
			    (form->formID == a_match.form.get_id()))
			{
				return false;
			}
		}

		if (a_match.keyword.get_id())
		{
			if (a_match.flags.test(Data::NodeOverrideConditionFlags::kNegateMatch2) ==
			    IFormCommon::HasKeyword(form, a_match.keyword))
			{
				return false;
			}
		}

		return true;
	}

	bool match_equipped_type(
		const Data::configNodeOverrideCondition_t& a_match,
		INodeOverride::nodeOverrideParams_t&       a_params)
	{
		TESForm* form;

		if (Conditions::is_hand_slot(a_match.typeSlot))
		{
			auto pm = a_params.actor->processManager;
			if (!pm)
			{
				return false;
			}

			auto isLeftSlot = Data::ItemData::IsLeftHandExtraSlot(a_match.typeSlot);

			form = pm->equippedObject[isLeftSlot ? ActorProcessManager::kEquippedHand_Left : ActorProcessManager::kEquippedHand_Right];

			if (!form)
			{
				return false;
			}

			if (!Conditions::is_valid_form_for_slot(form, a_match.typeSlot, isLeftSlot))
			{
				return false;
			}
		}
		else
		{
			if (a_match.typeSlot == Data::ObjectSlotExtra::kArmor)
			{
				if (!a_params.get_biped_has_armor())
				{
					return false;
				}

				if (a_match.form.get_id())
				{
					auto data = a_params.get_item_data();

					auto it = data->find(a_match.form.get_id());

					auto rv = a_match.flags.test(Data::NodeOverrideConditionFlags::kNegateMatch1);

					if (it == data->end())
					{
						return rv;
					}

					auto armor = it->second.item->As<TESObjectARMO>();

					if (!armor || armor->IsShield())
					{
						return rv;
					}

					if (a_match.keyword.get_id())
					{
						if (a_match.flags.test(Data::NodeOverrideConditionFlags::kNegateMatch2) ==
						    IFormCommon::HasKeyword(armor, a_match.keyword))
						{
							return false;
						}
					}

					it->second.matched = !rv;

					return !rv;
				}
				else
				{
					if (a_match.keyword.get_id())
					{
						return a_match.flags.test(Data::NodeOverrideConditionFlags::kNegateMatch2) ==
						       a_params.equipped_armor_visitor([&](auto* a_form) {
								   return IFormCommon::HasKeyword(a_form, a_match.keyword);
							   });
					}

					return true;
				}
			}
			else if (a_match.typeSlot == Data::ObjectSlotExtra::kAmmo)
			{
				auto biped = a_params.get_biped();
				if (!biped)
				{
					return false;
				}

				auto& e = biped->get_object(BIPED_OBJECT::kQuiver);

				form = e.item;

				if (!form || form == e.addon)
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		if (a_match.form.get_id())
		{
			if (a_match.flags.test(Data::NodeOverrideConditionFlags::kNegateMatch1) ==
			    (a_match.form.get_id() == form->formID))
			{
				return false;
			}
		}

		if (a_match.keyword.get_id())
		{
			if (a_match.flags.test(Data::NodeOverrideConditionFlags::kNegateMatch2) ==
			    IFormCommon::HasKeyword(form, a_match.keyword))
			{
				return false;
			}
		}

		return true;
	}

	static bool match(
		const Data::configNodeOverrideCondition_t& a_data,
		INodeOverride::nodeOverrideParams_t&       a_params,
		bool                                       a_ignoreNode = false)
	{
		switch (a_data.fbf.type)
		{
		case Data::NodeOverrideConditionType::Form:
			{
				if (!a_data.flags.test_any(Data::NodeOverrideConditionFlags::kMatchAll))
				{
					return false;
				}

				auto& formid = a_data.form.get_id();

				if (!formid)
				{
					return false;
				}

				std::uint32_t result = 0;
				std::uint32_t min    = a_data.flags.test(Data::NodeOverrideConditionFlags::kMatchAll) &&
                                            !a_data.flags.test(Data::NodeOverrideConditionFlags::kMatchCategoryOperOR) ?
				                           2u :
                                           1u;

				if (a_data.flags.test(Data::NodeOverrideConditionFlags::kMatchSlots))
				{
					result += match_form_slot(a_data, a_params);

					if (result == min)
					{
						return true;
					}
				}

				if (a_data.flags.test(Data::NodeOverrideConditionFlags::kMatchEquipped))
				{
					result += match_form_equipped(a_data, a_params);
				}

				return result == min;
			}
			break;
		case Data::NodeOverrideConditionType::Keyword:
			{
				if (!a_data.flags.test_any(Data::NodeOverrideConditionFlags::kMatchAll))
				{
					return false;
				}

				auto keyword = a_data.keyword.get_form<BGSKeyword>();
				if (!keyword)
				{
					return false;
				}

				std::uint32_t result = 0;
				std::uint32_t min    = a_data.flags.test(Data::NodeOverrideConditionFlags::kMatchAll) &&
                                            !a_data.flags.test(Data::NodeOverrideConditionFlags::kMatchCategoryOperOR) ?
				                           2u :
                                           1u;

				if (a_data.flags.test(Data::NodeOverrideConditionFlags::kMatchSlots))
				{
					result += match_keyword_slot(keyword, a_params);

					if (result == min)
					{
						return true;
					}
				}

				if (a_data.flags.test(Data::NodeOverrideConditionFlags::kMatchEquipped))
				{
					result += match_keyword_equipped(keyword, a_params);
				}

				return result == min;
			}
			break;
		case Data::NodeOverrideConditionType::Type:
			{
				if (!a_data.flags.test_any(Data::NodeOverrideConditionFlags::kMatchAll))
				{
					return false;
				}

				if (a_data.typeSlot >= Data::ObjectSlotExtra::kMax)
				{
					return false;
				}

				std::uint32_t result = 0;
				std::uint32_t min    = a_data.flags.test(Data::NodeOverrideConditionFlags::kMatchAll) &&
                                            !a_data.flags.test(Data::NodeOverrideConditionFlags::kMatchCategoryOperOR) ?
				                           2u :
                                           1u;

				if (a_data.flags.test(Data::NodeOverrideConditionFlags::kMatchSlots))
				{
					result += match_slotted_type(a_data, a_params);

					if (result == min)
					{
						return true;
					}
				}

				if (a_data.flags.test(Data::NodeOverrideConditionFlags::kMatchEquipped))
				{
					result += match_equipped_type(a_data, a_params);
				}

				return result == min;
			}
			break;
		case Data::NodeOverrideConditionType::BipedSlot:
			{
				return Conditions::match_biped<
					Data::configNodeOverrideCondition_t,
					Data::NodeOverrideConditionFlags>(
					a_params,
					a_data,
					[&](TESForm* a_form) {
						if (a_form->IsArmor())
						{
							auto data = a_params.get_item_data();
							auto it   = data->find(a_form->formID);
							if (it != data->end())
							{
								it->second.matched = true;
							}
						}
					});
			}
			break;
		case Data::NodeOverrideConditionType::Node:
			{
				if (a_ignoreNode)
				{
					break;
				}

				auto& cme = a_params.objects.GetCMENodes();

				auto it = cme.find(a_data.node);
				if (it == cme.end())
				{
					return false;
				}

				if (a_data.flags.test(Data::NodeOverrideConditionFlags::kExtraFlag0))
				{
					auto sh = a_params.controller.GetBSStringHolder();
					if (!it->second.has_visible_geometry(sh->m_scb, sh->m_scbLeft))
					{
						return false;
					}
				}
				else
				{
					if (!it->second.has_visible_geometry())
					{
						return false;
					}
				}

				return true;
			}
			break;
		case Data::NodeOverrideConditionType::Race:

			return Conditions::match_race<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(a_params, a_data);

		case Data::NodeOverrideConditionType::Actor:

			return Conditions::match_form(a_data.form.get_id(), a_params.actor);

		case Data::NodeOverrideConditionType::NPC:

			return Conditions::match_form(a_data.form.get_id(), a_params.npc);

		case Data::NodeOverrideConditionType::Furniture:

			return Conditions::match_furniture<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(a_params, a_data);

		case Data::NodeOverrideConditionType::Extra:

			return Conditions::match_extra<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(a_params, a_data);

		case Data::NodeOverrideConditionType::Location:

			return Conditions::match_location<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(a_params, a_data);

		case Data::NodeOverrideConditionType::Worldspace:

			return Conditions::match_worldspace<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(a_params, a_data);

		case Data::NodeOverrideConditionType::Package:

			return Conditions::match_package<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(a_params, a_data);

		case Data::NodeOverrideConditionType::Weather:

			return Conditions::match_weather<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(a_params, a_data);
		}

		return false;
	}

	static constexpr bool run_matches(
		const Data::configNodeOverrideConditionList_t& a_data,
		INodeOverride::nodeOverrideParams_t&           a_params,
		bool                                           a_default,
		bool                                           a_ignoreNode = false)
	{
		bool result = a_default;

		for (auto& e : a_data)
		{
			if (e.fbf.type == Data::NodeOverrideConditionType::Group)
			{
				result = run_matches(e.group.conditions, a_params, a_default, a_ignoreNode);
			}
			else
			{
				result = match(e, a_params, a_ignoreNode);
			}

			if (e.flags.test(Data::NodeOverrideConditionFlags::kNot))
			{
				result = !result;
			}

			if (e.flags.test(Data::NodeOverrideConditionFlags::kAnd))
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

	static constexpr bool run_matches(
		const Data::configNodeOverrideOffset_t& a_data,
		INodeOverride::nodeOverrideParams_t&    a_params)
	{
		return run_matches(
			a_data.conditions,
			a_params,
			!a_data.offsetFlags.test(Data::NodeOverrideOffsetFlags::kRequiresConditionList));
	}

	static constexpr bool run_visibility_matches(
		const Data::configNodeOverrideTransform_t& a_data,
		INodeOverride::nodeOverrideParams_t&       a_params)
	{
		return run_matches(
			a_data.visibilityConditionList,
			a_params,
			!a_data.overrideFlags.test(Data::NodeOverrideFlags::kVisibilityRequiresConditionList),
			true);
	}

	void constexpr apply_transform(
		const Data::configNodeOverrideOffset_t& a_data,
		NiTransform&                            a_out,
		NiPoint3&                               a_posAccum)
	{
		auto& xfrm = a_data.transform;

		if (xfrm.scale)
		{
			a_out.scale = std::clamp(Math::zero_nan(a_out.scale * *xfrm.scale), 0.01f, 100.0f);
		}

		if (xfrm.rotation)
		{
			NiMatrix33 rot(
				xfrm.rotation->x,
				xfrm.rotation->y,
				xfrm.rotation->z);

			a_out.rot = a_out.rot * rot;
		}

		if (xfrm.position)
		{
			NiPoint3 pos = *xfrm.position;

			if (a_data.offsetFlags.test(Data::NodeOverrideOffsetFlags::kLockToAccum))
			{
				if (a_posAccum.x < 0.0f)
				{
					pos.x = std::min(pos.x - a_posAccum.x, 0.0f);
				}
				else if (a_posAccum.x > 0.0f)
				{
					pos.x = std::max(pos.x - a_posAccum.x, 0.0f);
				}

				if (a_posAccum.y < 0.0f)
				{
					pos.y = std::min(pos.y - a_posAccum.y, 0.0f);
				}
				else if (a_posAccum.y > 0.0f)
				{
					pos.y = std::max(pos.y - a_posAccum.y, 0.0f);
				}

				if (a_posAccum.z < 0.0f)
				{
					pos.z = std::min(pos.z - a_posAccum.z, 0.0f);
				}
				else if (a_posAccum.z > 0.0f)
				{
					pos.z = std::max(pos.z - a_posAccum.z, 0.0f);
				}
			}

			if (a_data.flags.test(Data::NodeOverrideValuesFlags::kAbsolutePosition))
			{
				a_out.pos += pos;

				if (a_data.offsetFlags.test(Data::NodeOverrideOffsetFlags::kAccumulatePos))
				{
					a_posAccum += pos;
				}
			}
			else
			{
				auto apos = (a_out.rot * pos) * a_out.scale;

				a_out.pos += apos;

				if (a_data.offsetFlags.test(Data::NodeOverrideOffsetFlags::kAccumulatePos))
				{
					a_posAccum += apos;
				}
			}
		}
	}

	void apply_adjust(
		const Data::configNodeOverrideOffset_t& a_data,
		NiTransform&                            a_out,
		float                                   a_adjust,
		NiPoint3&                               a_posAccum)
	{
		NiPoint3 offset;

		if (a_data.offsetFlags.test(Data::NodeOverrideOffsetFlags::kAdjustX))
		{
			offset.x = a_adjust * a_data.adjustScale.x;
		}

		if (a_data.offsetFlags.test(Data::NodeOverrideOffsetFlags::kAdjustY))
		{
			offset.y = a_adjust * a_data.adjustScale.y;
		}

		if (a_data.offsetFlags.test(Data::NodeOverrideOffsetFlags::kAdjustZ))
		{
			offset.z = a_adjust * a_data.adjustScale.z;
		}

		if (a_data.offsetFlags.test(Data::NodeOverrideOffsetFlags::kAccumulatePos))
		{
			a_posAccum += offset;
		}

		if (a_data.flags.test(Data::NodeOverrideValuesFlags::kAbsolutePosition))
		{
			a_out.pos += offset;
		}
		else
		{
			a_out.pos += (a_out.rot * offset) * a_out.scale;
		}
	}

	void INodeOverride::ApplyNodeOverride(
		const cmeNodeEntry_t&                      a_entry,
		const Data::configNodeOverrideTransform_t& a_data,
		nodeOverrideParams_t&                      a_params)
	{
		NiTransform xfrm;

		if (a_data.transform.scale)
		{
			xfrm.scale = std::clamp(Math::zero_nan(xfrm.scale * *a_data.transform.scale), 0.01f, 100.0f);
		}

		if (a_data.transform.rotation)
		{
			xfrm.rot.SetEulerAngles(
				a_data.transform.rotation->x,
				a_data.transform.rotation->y,
				a_data.transform.rotation->z);
		}

		if (a_data.transform.position)
		{
			if (a_data.flags.test(Data::NodeOverrideValuesFlags::kAbsolutePosition))
			{
				xfrm.pos = *a_data.transform.position;
			}
			else
			{
				xfrm.pos = (xfrm.rot * *a_data.transform.position) * xfrm.scale;
			}
		}

		NiPoint3 accumPos;

		process_offsets(a_data.offsets, xfrm, accumPos, a_params);

		bool update = std::memcmp(
						  std::addressof(a_entry.node->m_localTransform),
						  std::addressof(xfrm),
						  sizeof(NiTransform)) != 0;

		a_entry.node->m_localTransform = xfrm;

		if (update)
		{
			NiAVObject::ControllerUpdateContext ctx{ 0, 0 };
			a_entry.node->UpdateDownwardPass(ctx, nullptr);
		}
	}

	void INodeOverride::ResetNodeOverrideImpl(NiAVObject* a_object)
	{
		a_object->m_localTransform = {};
		a_object->SetVisible(true);
	}

	void INodeOverride::ResetNodeOverride(
		const cmeNodeEntry_t& a_entry)
	{
		if (EngineExtensions::SceneRendering() ||
		    !ITaskPool::IsRunningOnCurrentThread())
		{
			ITaskPool::AddPriorityTask(
				[node = a_entry.node]() {
					ResetNodeOverrideImpl(node);
				});
		}
		else
		{
			ResetNodeOverrideImpl(a_entry.node);
		}
	}

	bool INodeOverride::process_offsets(
		const Data::configNodeOverrideOffsetList_t& a_data,
		NiTransform&                                a_out,
		NiPoint3&                                   a_posAccum,
		nodeOverrideParams_t&                       a_params)
	{
		bool matched = false;

		for (auto& e : a_data)
		{
			if (run_matches(e, a_params))
			{
				if (e.offsetFlags.test(Data::NodeOverrideOffsetFlags::kIsGroup))
				{
					if (process_offsets(e.group, a_out, a_posAccum, a_params))
					{
						matched = true;

						if (!e.offsetFlags.test(Data::NodeOverrideOffsetFlags::kContinue))
						{
							break;
						}
					}
				}
				else
				{
					matched = true;

					a_params.clear_matched_items();

					if (e.offsetFlags.test_any(Data::NodeOverrideOffsetFlags::kAdjustFlags))
					{
						if (!(e.offsetFlags.test(Data::NodeOverrideOffsetFlags::kAdjustIgnoreDead) && a_params.get_actor_dead()))
						{
							float adjust = 0.0f;

							if (e.offsetFlags.test(Data::NodeOverrideOffsetFlags::kWeightAdjust))
							{
								adjust += a_params.get_weight_adjust();
							}

							if (e.offsetFlags.test(Data::NodeOverrideOffsetFlags::kWeaponAdjust))
							{
								if (e.offsetFlags.test(Data::NodeOverrideOffsetFlags::kWeaponAdjustMatchedOnly))
								{
									adjust += a_params.get_matched_weapon_adjust();
								}
								else
								{
									adjust += a_params.get_weapon_adjust();
								}
							}

							apply_adjust(e, a_out, std::clamp(adjust, 0.0f, 1.0f) * 4.0f, a_posAccum);
						}
					}
					else
					{
						apply_transform(e, a_out, a_posAccum);
					}

					if (e.offsetFlags.test(Data::NodeOverrideOffsetFlags::kResetAccum))
					{
						a_posAccum = {};
					}

					if (!e.offsetFlags.test(Data::NodeOverrideOffsetFlags::kContinue))
					{
						break;
					}
				}
			}
		}

		return matched;
	}

	void INodeOverride::ApplyNodeVisibility(
		NiNode*                                    a_node,
		const Data::configNodeOverrideTransform_t& a_data,
		nodeOverrideParams_t&                      a_params)
	{
		bool visible = true;

		if (run_visibility_matches(a_data, a_params))
		{
			visible = !a_data.overrideFlags.test(Data::NodeOverrideFlags::kInvisible);
		}

		a_node->SetVisible(visible);
	}

	void INodeOverride::attach_node_to(
		const weapNodeEntry_t&   a_entry,
		const NiPointer<NiNode>& a_target)
	{
		if (a_target &&
		    a_entry.node->m_parent &&
		    a_entry.node->m_parent != a_target)
		{
			if (EngineExtensions::SceneRendering() ||
			    !ITaskPool::IsRunningOnCurrentThread())
			{
				ITaskPool::AddPriorityTask(
					[target = a_target,
				     node   = a_entry.node]() {
						if (node->m_parent &&
					        node->m_parent != target)
						{
							target->AttachChild(node, true);

							NiAVObject::ControllerUpdateContext ctx{ 0, 0 };
							node->UpdateDownwardPass(ctx, nullptr);
						}
					});
			}
			else
			{
				a_target->AttachChild(a_entry.node, true);

				NiAVObject::ControllerUpdateContext ctx{ 0, 0 };
				a_entry.node->UpdateDownwardPass(ctx, nullptr);
			}
		}
	}

	void INodeOverride::ApplyNodePlacement(
		const Data::configNodeOverridePlacement_t& a_data,
		const weapNodeEntry_t&                     a_entry,
		nodeOverrideParams_t&                      a_params)
	{
		auto& target = get_target_node(
			a_data,
			a_entry,
			a_params);

		if (!target.empty())
		{
			auto& mdata = a_params.objects.GetMOVNodes();

			if (auto it = mdata.find(target); it != mdata.end())
			{
				if (a_entry.target != it->second.node)
				{
					a_entry.target = it->second.node;
				}

				attach_node_to(a_entry, a_entry.target);
			}
		}
		else
		{
			ResetNodePlacement(a_entry);
		}
	}

	constexpr auto INodeOverride::get_target_node(
		const Data::configNodeOverridePlacement_t& a_data,
		const weapNodeEntry_t&                     a_entry,
		nodeOverrideParams_t&                      a_params)
		-> const stl::fixed_string&
	{
		for (auto& e : a_data.overrides)
		{
			if (run_matches(
					e.conditions,
					a_params,
					false,
					true))
			{
				return e.targetNode;
			}
		}

		return a_data.targetNode;
	}

	void INodeOverride::ResetNodePlacement(
		const weapNodeEntry_t& a_entry)
	{
		if (a_entry.target != nullptr)
		{
			a_entry.target.reset();
			attach_node_to(a_entry, a_entry.defaultNode);
		}
	}

	auto INodeOverride::nodeOverrideParams_t::get_item_data()
		-> item_container_type*
	{
		if (!itemData)
		{
			itemData = std::make_unique<decltype(itemData)::element_type>();

			if (auto data = get_biped())
			{
				using enum_type = std::underlying_type_t<BIPED_OBJECT>;

				for (enum_type i = stl::underlying(BIPED_OBJECT::kHead); i < stl::underlying(BIPED_OBJECT::kTotal); i++)
				{
					auto item = data->objects[i].item;
					if (!item)
					{
						continue;
					}

					auto addon = data->objects[i].addon;
					if (addon == item)
					{
						continue;
					}

					auto r = itemData->try_emplace(
						item->formID,
						item,
						static_cast<BIPED_OBJECT>(i));

					if (addon)
					{
						if (auto armor = item->As<TESObjectARMO>())
						{
							if (auto arma = addon->As<TESObjectARMA>())
							{
								r.first->second.weaponAdjust = std::max(
									Math::zero_nan(arma->data.weaponAdjust),
									r.first->second.weaponAdjust);
							}
						}
					}
				}
			}
		}

		return itemData.get();
	}

	float INodeOverride::nodeOverrideParams_t::get_weapon_adjust()
	{
		if (!weaponAdjust)
		{
			weaponAdjust = 0.0f;

			if (auto data = get_biped())
			{
				for (auto& e : data->objects)
				{
					if (e.addon && e.addon != e.item)
					{
						if (auto arma = e.addon->As<TESObjectARMA>())
						{
							*weaponAdjust = std::max(
								Math::zero_nan(arma->data.weaponAdjust),
								*weaponAdjust);
						}
					}
				}
			}
		}

		return *weaponAdjust;
	}
}