#include "pch.h"

#include "Controller.h"

#include "IED/ConditionsCommon.h"
#include "IED/ConfigNodeOverride.h"
#include "IED/EngineExtensions.h"
#include "IED/FormCommon.h"
#include "IED/GearNodeData.h"
#include "IED/Inventory.h"
#include "INodeOverride.h"
#include "ObjectManagerData.h"

namespace IED
{
	using namespace ::Util::Node;

	static constexpr bool match_form_slot(
		const Data::configNodeOverrideCondition_t& a_data,
		const nodeOverrideParams_t&                a_params) noexcept
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
		BGSKeyword*                 a_keyword,
		const nodeOverrideParams_t& a_params) noexcept
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

	static constexpr bool match_keyword_equipped(
		BGSKeyword*           a_keyword,
		nodeOverrideParams_t& a_params) noexcept
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

		auto& data = a_params.get_item_data();

		for (auto& e : data)
		{
			if (IFormCommon::HasKeyword(e.second.item, a_keyword))
			{
				if (e.second.item->IsArmor())
				{
					a_params.set_matched_item(e.second.bip);
				}

				return true;
			}
		}

		return false;
	}

	static auto find_equipped_form(
		Game::FormID          a_form,
		nodeOverrideParams_t& a_params) noexcept
		-> std::pair<TESForm*, nodeOverrideParams_t::item_container_type::value_type*>
	{
		if (auto form = Conditions::match_pm_equipped(a_params.actor, a_form))
		{
			return { form, nullptr };
		}

		auto& data = a_params.get_item_data();

		auto it = data.find(a_form);
		if (it != data.end())
		{
			return { it->second.item, std::addressof(*it) };
		}

		return { nullptr, nullptr };
	}

	static bool match_form_equipped(
		const Data::configNodeOverrideCondition_t& a_data,
		nodeOverrideParams_t&                      a_params) noexcept
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
			a_params.set_matched_item(r.second->second.bip);
		}

		return true;
	}

	static constexpr bool match_slotted_type(
		const Data::configNodeOverrideCondition_t& a_match,
		nodeOverrideParams_t&                      a_params) noexcept
	{
		const auto slot = Data::ItemData::ExtraSlotToSlot(a_match.typeSlot);
		if (slot >= Data::ObjectSlot::kMax)
		{
			return false;
		}

		const auto form = a_params.objects.GetSlot(slot).GetFormIfActive();
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

		if (a_match.flags.test(Data::NodeOverrideConditionFlags::kExtraFlag1))
		{
			if (a_match.flags.test(Data::NodeOverrideConditionFlags::kNegateMatch3) ==
			    (slot == Data::ObjectSlot::kAmmo && Conditions::is_ammo_bolt(form)))
			{
				return false;
			}
		}

		return true;
	}

	static bool match_equipped_type(
		const Data::configNodeOverrideCondition_t& a_match,
		nodeOverrideParams_t&                      a_params) noexcept
	{
		TESForm* form;

		if (Conditions::is_hand_slot(a_match.typeSlot))
		{
			if (a_match.flags.test(Data::NodeOverrideConditionFlags::kExtraFlag1))
			{
				return a_match.flags.test(Data::NodeOverrideConditionFlags::kNegateMatch3);
			}

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
				if (a_match.flags.test(Data::NodeOverrideConditionFlags::kExtraFlag1))
				{
					return a_match.flags.test(Data::NodeOverrideConditionFlags::kNegateMatch3);
				}

				if (!a_params.get_biped_has_armor())
				{
					return false;
				}

				if (const auto fid = a_match.form.get_id())
				{
					auto& data = a_params.get_item_data();

					auto it = data.find(fid);

					auto rv = a_match.flags.test(Data::NodeOverrideConditionFlags::kNegateMatch1);

					if (it == data.end())
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

					if (!rv)
					{
						a_params.set_matched_item(it->second.bip);
					}

					return !rv;
				}
				else
				{
					if (a_match.keyword.get_id())
					{
						return a_match.flags.test(Data::NodeOverrideConditionFlags::kNegateMatch2) ==
						       a_params.equipped_armor_visitor([&](auto* a_armor) {
								   return IFormCommon::HasKeyword(a_armor, a_match.keyword);
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

				if (a_match.flags.test(Data::NodeOverrideConditionFlags::kExtraFlag1))
				{
					if (a_match.flags.test(Data::NodeOverrideConditionFlags::kNegateMatch3) ==
					    Conditions::is_ammo_bolt(form))
					{
						return false;
					}
				}
			}
			else
			{
				if (a_match.flags.test(Data::NodeOverrideConditionFlags::kExtraFlag1))
				{
					return a_match.flags.test(Data::NodeOverrideConditionFlags::kNegateMatch3);
				}

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

	static bool evaluate_condition(
		const Data::configNodeOverrideCondition_t& a_data,
		nodeOverrideParams_t&                      a_params,
		bool                                       a_ignoreNode = false) noexcept
	{
		switch (a_data.flags.bf().type)
		{
		case Data::NodeOverrideConditionType::Form:
			{
				if (!a_data.flags.test_any(Data::NodeOverrideConditionFlags::kMatchAll))
				{
					return false;
				}

				if (!a_data.form.get_id())
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
					[&](const TESForm* a_form) {
						if (a_form->IsArmor())
						{
							auto& data = a_params.get_item_data();
							auto  it   = data.find(a_form->formID);

							if (it != data.end())
							{
								a_params.set_matched_item(it->second.bip);
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

				auto it = cme.find(a_data.s0);
				if (it == cme.end())
				{
					return false;
				}

				if (a_data.bipedSlot != BIPED_OBJECT::kNone)
				{
					const auto slot = a_params.translate_biped_object(a_data.bipedSlot);

					if (slot >= BIPED_OBJECT::kTotal)
					{
						return false;
					}

					auto biped = a_params.get_biped();
					if (!biped)
					{
						return false;
					}

					auto object = biped->get_object(a_data.bipedSlot).object.get();
					if (!object)
					{
						return false;
					}

					return it->second.has_visible_object(object);
				}
				else
				{
#if defined(IED_ENABLE_I3DI)
					/*if (a_params.objects.GetNodeConditionForced())
					{
						auto& wnodes = a_params.objects.GetWeapNodes();

						auto r = ::Util::Node::Traverse(it->second.thirdPerson.node.get(), [&](NiAVObject* a_object) {
							auto it = std::find_if(wnodes.begin(), wnodes.end(), [&](const auto& a_v) {
								return a_v.node3p.node == a_object;
							});

							return it != wnodes.end() ?
							           Util::Node::VisitorControl::kStop :
							           Util::Node::VisitorControl::kContinue;
						});

						return r == Util::Node::VisitorControl::kStop;
					}*/
#endif

					auto sh = a_data.flags.test(Data::NodeOverrideConditionFlags::kExtraFlag0) ?
					              BSStringHolder::GetSingleton() :
					              nullptr;

					return it->second.has_visible_geometry(sh);
				}
			}
			break;

		case Data::NodeOverrideConditionType::Race:

			return Conditions::match_race<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(a_params, a_data);

		case Data::NodeOverrideConditionType::Actor:

			return Conditions::match_actor<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(a_params, a_data);

		case Data::NodeOverrideConditionType::NPC:

			return Conditions::match_npc<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(a_params, a_data);

		case Data::NodeOverrideConditionType::Furniture:

			return Conditions::match_furniture<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(a_params, a_data);

		case Data::NodeOverrideConditionType::Extra:

			return Conditions::match_extra<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(
				a_params,
				a_data,
				a_params.objects.GetCachedData());

		case Data::NodeOverrideConditionType::Location:

			return Conditions::match_location<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(a_params, a_data);

		case Data::NodeOverrideConditionType::Worldspace:

			return Conditions::match_worldspace<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(
				a_params,
				a_data,
				a_params.objects.GetCachedData());

		case Data::NodeOverrideConditionType::Package:

			return Conditions::match_package<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(
				a_params,
				a_data,
				a_params.objects.GetCachedData());

		case Data::NodeOverrideConditionType::Weather:

			return Conditions::match_weather<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(a_params, a_data);

		case Data::NodeOverrideConditionType::Global:

			return Conditions::match_global<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(a_params, a_data);

		case Data::NodeOverrideConditionType::Mounting:

			return Conditions::match_mount<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(a_params, a_data);

		case Data::NodeOverrideConditionType::Mounted:

			return Conditions::match_mounted_by<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(
				a_params,
				a_data,
				a_params.objects.GetCachedData());

		case Data::NodeOverrideConditionType::Idle:

			return Conditions::match_idle<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(
				a_params,
				a_data,
				a_params.objects.GetCachedData());

		case Data::NodeOverrideConditionType::Skeleton:

			return Conditions::match_skeleton<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(
				a_params,
				a_data);

		case Data::NodeOverrideConditionType::Faction:

			return Conditions::match_faction<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(
				a_params,
				a_data,
				a_params.objects.GetCachedData());

		case Data::NodeOverrideConditionType::Effect:

			return Conditions::match_effect<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(
				a_params,
				a_data,
				a_params.objects.GetCachedData());

		case Data::NodeOverrideConditionType::Variable:

			return Conditions::match_variable<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(
				a_params,
				a_data);

		case Data::NodeOverrideConditionType::Perk:

			return Conditions::match_perk<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(
				a_params,
				a_data,
				a_params.objects.GetCachedData());

		case Data::NodeOverrideConditionType::Cell:

			return Conditions::match_cell<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(
				a_params,
				a_data);

		case Data::NodeOverrideConditionType::Hand:

			return Conditions::match_hand_item<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(
				a_params,
				a_data);

		case Data::NodeOverrideConditionType::Voice:

			return Conditions::match_voice_type<
				Data::configNodeOverrideCondition_t,
				Data::NodeOverrideConditionFlags>(
				a_params,
				a_data);
		}

		return false;
	}

	static constexpr bool run_conditions(
		const Data::configNodeOverrideConditionList_t& a_data,
		nodeOverrideParams_t&                          a_params,
		bool                                           a_default,
		bool                                           a_ignoreNode = false) noexcept
	{
		bool result = a_default;

		for (auto& e : a_data)
		{
			if (e.flags.bf().type == Data::NodeOverrideConditionType::Group)
			{
				result = run_conditions(e.group.conditions, a_params, a_default, a_ignoreNode);
			}
			else
			{
				result = evaluate_condition(e, a_params, a_ignoreNode);
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

	static constexpr bool run_conditions(
		const Data::configNodeOverrideOffset_t& a_data,
		nodeOverrideParams_t&                   a_params) noexcept
	{
		return run_conditions(
			a_data.conditions,
			a_params,
			!a_data.offsetFlags.test(Data::NodeOverrideOffsetFlags::kRequiresConditionList));
	}

	static constexpr bool run_visibility_conditions(
		const Data::configNodeOverrideTransform_t& a_data,
		nodeOverrideParams_t&                      a_params) noexcept
	{
		return run_conditions(
			a_data.visibilityConditionList,
			a_params,
			!a_data.overrideFlags.test(Data::NodeOverrideFlags::kVisibilityRequiresConditionList),
			true);
	}

	static void constexpr apply_transform(
		const Data::configNodeOverrideOffset_t& a_data,
		NiTransform&                            a_out,
		NiPoint3&                               a_posAccum) noexcept
	{
		auto& xfrm = a_data.transform;

		if (xfrm.scale)
		{
			a_out.scale = std::clamp(a_out.scale * *xfrm.scale, 0.01f, 100.0f);
		}

		if (xfrm.rotationMatrix)
		{
			a_out.rot = a_out.rot * *xfrm.rotationMatrix;
		}

		if (xfrm.position)
		{
			auto pos = *xfrm.position;

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
				const auto apos = a_out.rot._simd_mulpt_scale(pos, a_out.scale);

				a_out.pos += apos;

				if (a_data.offsetFlags.test(Data::NodeOverrideOffsetFlags::kAccumulatePos))
				{
					a_posAccum += apos;
				}
			}
		}
	}

	static void apply_adjust(
		const Data::configNodeOverrideOffset_t& a_data,
		NiTransform&                            a_out,
		float                                   a_adjust,
		NiPoint3&                               a_posAccum) noexcept
	{
		const auto offset = a_data.adjustScale * a_adjust;

		if (a_data.flags.test(Data::NodeOverrideValuesFlags::kAbsolutePosition))
		{
			a_out.pos += offset;

			if (a_data.offsetFlags.test(Data::NodeOverrideOffsetFlags::kAccumulatePos))
			{
				a_posAccum += offset;
			}
		}
		else
		{
			const auto apos = a_out.rot._simd_mulpt_scale(offset, a_out.scale);

			a_out.pos += apos;

			if (a_data.offsetFlags.test(Data::NodeOverrideOffsetFlags::kAccumulatePos))
			{
				a_posAccum += apos;
			}
		}
	}

	void INodeOverride::ApplyNodeOverride(
		const stl::fixed_string&                   a_name,
		const CMENodeEntry&                        a_entry,
		const Data::configNodeOverrideTransform_t& a_data,
		nodeOverrideParams_t&                      a_params) noexcept
	{
		const stl::ftz_daz_ctl_scoped<_MM_FLUSH_ZERO_ON | _MM_DENORMALS_ZERO_ON> fds;

		auto xfrm = a_entry.thirdPerson.orig;

		if (a_data.transform.scale)
		{
			xfrm.scale = std::clamp(xfrm.scale * *a_data.transform.scale, 0.01f, 100.0f);
		}

		if (a_data.transform.rotationMatrix)
		{
			xfrm.rot = xfrm.rot * *a_data.transform.rotationMatrix;
		}

		if (a_data.transform.position)
		{
			if (a_data.flags.test(Data::NodeOverrideValuesFlags::kAbsolutePosition))
			{
				xfrm.pos = *a_data.transform.position;
			}
			else
			{
				xfrm.pos += xfrm.rot._simd_mulpt_scale(*a_data.transform.position, xfrm.scale);
			}
		}

		NiPoint3 accumPos;

		process_offsets(a_data.offsets, xfrm, accumPos, a_params);

		const bool update = std::memcmp(
								std::addressof(a_entry.thirdPerson.node->m_localTransform),
								std::addressof(xfrm),
								sizeof(NiTransform)) != 0;

		if (update)
		{
			a_entry.thirdPerson.node->m_localTransform = xfrm;
			UpdateDownwardPass(a_entry.thirdPerson.node);

			if (a_entry.firstPerson)
			{
				a_entry.firstPerson.node->m_localTransform = xfrm;
				UpdateDownwardPass(a_entry.firstPerson.node);
			}
		}
	}

	void INodeOverride::ResetNodeOverrideImpl(
		const CMENodeEntry::Node& a_node) noexcept
	{
		a_node.node->m_localTransform = a_node.orig;
		a_node.node->SetVisible(true);
	}

	void INodeOverride::ResetNodeOverride(
		const CMENodeEntry& a_entry,
		bool                a_defer) noexcept
	{
		if (a_defer)
		{
			ITaskPool::AddPriorityTask(
				[entry = a_entry]() noexcept {
					ResetNodeOverrideImpl(entry.thirdPerson);
					if (entry.firstPerson)
					{
						ResetNodeOverrideImpl(entry.firstPerson);
					}
				});
		}
		else
		{
			assert(!EngineExtensions::ShouldDefer3DTask());

			ResetNodeOverrideImpl(a_entry.thirdPerson);
			if (a_entry.firstPerson)
			{
				ResetNodeOverrideImpl(a_entry.firstPerson);
			}
		}
	}

	bool INodeOverride::process_offsets(
		const Data::configNodeOverrideOffsetList_t& a_data,
		NiTransform&                                a_out,
		NiPoint3&                                   a_posAccum,
		nodeOverrideParams_t&                       a_params) noexcept
	{
		bool matched = false;

		for (auto& e : a_data)
		{
			a_params.clear_matched_items();

			if (run_conditions(e, a_params))
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
		const CMENodeEntry&                        a_entry,
		const Data::configNodeOverrideTransform_t& a_data,
		nodeOverrideParams_t&                      a_params) noexcept
	{
		bool visible = true;

		if (run_visibility_conditions(a_data, a_params))
		{
			visible = !a_data.overrideFlags.test(Data::NodeOverrideFlags::kInvisible);
		}

		a_entry.thirdPerson.node->SetVisible(visible);
		if (a_entry.firstPerson)
		{
			a_entry.firstPerson.node->SetVisible(visible);
		}
	}

	static void try_attach_node_impl(
		const NiPointer<NiNode>& a_node,
		const NiPointer<NiNode>& a_target,
		bool                     a_defer) noexcept
	{
		if (a_target &&
		    a_node->m_parent &&
		    a_node->m_parent != a_target)
		{
			if (a_defer)
			{
				ITaskPool::AddPriorityTask(
					[target = a_target,
				     node   = a_node]() noexcept {
						if (node->m_parent &&
					        node->m_parent != target)
						{
							target->AttachChild(node, true);

							//UpdateDownwardPass(node);
						}
					});
			}
			else
			{
				assert(!EngineExtensions::ShouldDefer3DTask());

				a_target->AttachChild(a_node, true);

				UpdateDownwardPass(a_node);
			}
		}
	}

	static void try_attach_node_to(
		const GearNodeEntry&     a_entry,
		const NiPointer<NiNode>& a_target,
		const NiPointer<NiNode>& a_target1p,
		bool                     a_defer) noexcept
	{
		try_attach_node_impl(a_entry.node3p.node, a_target, a_defer);

		if (a_entry.node1p)
		{
			try_attach_node_impl(a_entry.node1p.node, a_target1p, a_defer);
		}
	}

	static constexpr void try_update_anim_placement_id(
		const GearNodeEntry&  a_entry,
		nodeOverrideParams_t* a_params,
		WeaponPlacementID     a_placementID) noexcept
	{
		if (a_params && a_entry.animSlot < AnimationWeaponSlot::Max)
		{
			auto& state = a_params->objects.GetAnimState();

			const auto index = stl::underlying(a_entry.animSlot);
			if (state.placement[index] != a_placementID)
			{
				state.placement[index] = a_placementID;
				state.flags.set(ActorAnimationState::Flags::kNeedUpdate);
			}
		}
	}

	void INodeOverride::ApplyNodePlacement(
		const Data::configNodeOverridePlacement_t& a_data,
		const GearNodeEntry&                       a_entry,
		nodeOverrideParams_t&                      a_params) noexcept
	{
		auto& target = get_target_node(
			a_data,
			a_params);

		if (!target.empty())
		{
			auto& mdata = a_params.objects.GetMOVNodes();

			if (auto it = mdata.find(target); it != mdata.end())
			{
				auto& e = it->second;

				try_attach_node_to(
					a_entry,
					e.thirdPerson.node,
					e.firstPerson.node,
					false);

				try_update_anim_placement_id(
					a_entry,
					std::addressof(a_params),
					e.placementID);

				GearNodeData::GetSingleton().SetPlacement(
					a_params.actor->formID,
					a_entry.gearNodeID,
					e.placementID);
			}
		}
		else
		{
			ResetNodePlacement(a_entry, std::addressof(a_params), false);
		}
	}

	const Data::configNodePhysicsValues_t& INodeOverride::GetPhysicsConfig(
		const Data::configNodeOverridePhysics_t& a_data,
		nodeOverrideParams_t&                    a_params) noexcept
	{
		for (auto& e : a_data.overrides)
		{
			if (run_conditions(
					e.conditions,
					a_params,
					false,
					false))
			{
				return e;
			}
		}

		return a_data;
	}

	constexpr auto INodeOverride::get_target_node(
		const Data::configNodeOverridePlacement_t& a_data,
		nodeOverrideParams_t&                      a_params) noexcept
		-> const stl::fixed_string&
	{
		for (auto& e : a_data.overrides)
		{
			if (run_conditions(
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
		const GearNodeEntry&  a_entry,
		nodeOverrideParams_t* a_params,
		bool                  a_defer) noexcept
	{
		//a_entry.target.reset();
		//a_entry.target1p.reset();

		try_attach_node_to(
			a_entry,
			a_entry.node3p.defaultParentNode,
			a_entry.node1p.defaultParentNode,
			a_defer);

		try_update_anim_placement_id(
			a_entry,
			a_params,
			WeaponPlacementID::Default);

		if (a_params)
		{
			GearNodeData::GetSingleton().SetPlacement(
				a_params->actor->formID,
				a_entry.gearNodeID,
				WeaponPlacementID::None);
		}
	}

}