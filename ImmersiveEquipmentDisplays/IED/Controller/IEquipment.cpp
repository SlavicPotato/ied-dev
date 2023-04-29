#include "pch.h"

#include "IEquipment.h"

#include "ActorObjectHolder.h"
#include "BipedSlotData.h"
#include "IRNG.h"

#include "IED/ProcessParams.h"

namespace IED
{
	using namespace Data;

	IEquipment::IEquipment(RandomNumberGeneratorBase& a_rng) noexcept :
		m_rng(a_rng)
	{
	}

	auto IEquipment::CreateEquippedItemInfo(
		const Actor* a_actor) noexcept
		-> EquippedItemInfo
	{
		const auto pm = a_actor->processManager;

		const auto formLeft  = pm ? pm->equippedObject[ActorProcessManager::kEquippedHand_Left] : nullptr;
		const auto formRight = pm ? pm->equippedObject[ActorProcessManager::kEquippedHand_Right] : nullptr;

		return EquippedItemInfo{
			formRight,
			formLeft,
			formRight ?
				ItemData::GetObjectSlotNoBound(formRight) :
				ObjectSlot::kMax,
			formLeft ?
				ItemData::GetObjectSlotLeftNoBound(formLeft) :
				ObjectSlot::kMax,
		};
	}

	auto IEquipment::SelectSlotItem(
		ProcessParams&          a_params,
		const Data::configSlot_t& a_config,
		SlotItemCandidates&       a_candidates,
		const ObjectEntrySlot&    a_slot) noexcept
		-> SelectedItem
	{
		if (a_candidates.empty())
		{
			return {};
		}

		const bool checkCannotWear =
			a_config.slotFlags.test(Data::SlotFlags::kCheckCannotWear);

		for (auto& e : a_config.preferredItems)
		{
			auto it = std::find_if(
				a_candidates.begin(),
				a_candidates.end(),
				[&](const auto& a_item) [[msvc::forceinline]] {
					return e == a_item.item->form->formID;
				});

			if (it != a_candidates.end())
			{
				if (!checkCannotWear ||
				    !it->item->cannot_wear())
				{
					return { it };
				}
			}
		}

		const auto lastEquipped = a_slot.slotState.lastEquipped;

		if (lastEquipped &&
		    a_config.itemFilter.test(lastEquipped))
		{
			auto it = std::find_if(
				a_candidates.begin(),
				a_candidates.end(),
				[&](const auto& a_item) [[msvc::forceinline]] {
					return a_item.item->form->formID == lastEquipped;
				});

			if (it != a_candidates.end())
			{
				if ((!checkCannotWear || !it->item->cannot_wear()) &&
				    configBase_t::do_match_fp(
						a_config.itemFilterCondition,
						{ it->item->form, a_slot.slotidex, a_slot.slotid },
						a_params,
						true))
				{
					return { it };
				}
			}
		}

		auto it = std::find_if(
			a_candidates.begin(),
			a_candidates.end(),
			[&](const auto& a_item) [[msvc::forceinline]] {
				const auto* const item = a_item.item;

				if (checkCannotWear && item->cannot_wear())
				{
					return false;
				}

				if (!a_config.itemFilter.test(item->form->formID))
				{
					return false;
				}

				return configBase_t::do_match_fp(
					a_config.itemFilterCondition,
					{ item->form, a_slot.slotidex, a_slot.slotid },
					a_params,
					true);
			});

		if (it != a_candidates.end())
		{
			return { it };
		}
		else
		{
			return {};
		}
	}

	static constexpr bool is_non_shield_armor(const TESForm* a_form) noexcept
	{
		if (auto armor = a_form->As<TESObjectARMO>())
		{
			return !armor->IsShield();
		}
		else
		{
			return false;
		}
	}

	bool IEquipment::CustomEntryValidateInventoryForm(
		ProcessParams&               a_params,
		const CollectorData::ItemData& a_itemData,
		const configCustom_t&          a_config,
		bool&                          a_hasMinCount) noexcept
	{
		const auto* const form = a_itemData.form;

		if (form->formID.IsTemporary())
		{
			return false;
		}

		if (a_itemData.itemCount <= 0)
		{
			return false;
		}

		if ((a_config.countRange.min && a_itemData.itemCount < a_config.countRange.min) ||
		    a_config.countRange.max && a_itemData.itemCount > a_config.countRange.max)
		{
			return false;
		}

		if (a_config.customFlags.test(CustomFlags::kCheckFav) &&
		    a_params.objects.IsPlayer() &&
		    !a_itemData.is_favorited())
		{
			return false;
		}

		if (a_config.customFlags.test_any(CustomFlags::kEquipmentModeMask))
		{
			const bool isAmmo = form->IsAmmo();

			if (!isAmmo &&
			    !a_config.customFlags.test(CustomFlags::kIgnoreRaceEquipTypes) &&
			    !is_non_shield_armor(form) &&
			    !a_params.objects.IsPlayer())
			{
				if (!a_params.test_equipment_flags(
						ItemData::GetRaceEquipmentFlagFromType(a_itemData.extra.type)))
				{
					return false;
				}
			}

			if (isAmmo)
			{
				a_hasMinCount = !a_itemData.is_equipped() && a_itemData.sharedCount > 0;
			}
			else
			{
				if (a_config.customFlags.test(CustomFlags::kDisableIfEquipped) &&
				    a_itemData.is_equipped())
				{
					a_hasMinCount = false;
				}
				else
				{
					const std::int64_t sharedCount = a_itemData.sharedCount;

					a_hasMinCount = (sharedCount - a_itemData.get_equip_count()) > 0;
				}
			}
		}
		else
		{
			a_hasMinCount = true;
		}

		return true;
	}

	CollectorData::container_type::const_iterator IEquipment::CustomEntrySelectInventoryFormGroup(
		ProcessParams&            a_params,
		const Data::configCustom_t& a_config,
		ObjectEntryCustom&          a_objectEntry,
		bool&                       a_hasMinCount) noexcept
	{
		const auto& formData = a_params.collector.data.forms;

		if (const auto& fid = a_config.form.get_id())
		{
			if (auto it = formData.find(fid); it != formData.end())
			{
				if (CustomEntryValidateInventoryForm(
						a_params,
						it->second,
						a_config,
						a_hasMinCount))
				{
					return it;
				}
			}
		}

		return formData.end();
	}

	template <class Tf>
	CollectorData::container_type::const_iterator IEquipment::CustomEntrySelectInventoryFormDefault(
		ProcessParams&            a_params,
		const Data::configCustom_t& a_config,
		ObjectEntryCustom&          a_objectEntry,
		bool&                       a_hasMinCount,
		Tf                          a_filter) noexcept
	{
		const auto& formData = a_params.collector.data.forms;

		if (a_config.customFlags.test(CustomFlags::kSelectInvRandom) &&
		    !a_config.extraItems.empty())
		{
			if (a_objectEntry.data.state)
			{
				const auto fid = a_objectEntry.data.state->form->formID;

				if (fid == a_config.form.get_id() ||
				    std::find(
						a_config.extraItems.begin(),
						a_config.extraItems.end(),
						fid) != a_config.extraItems.end())
				{
					if (auto it = formData.find(fid); it != formData.end())
					{
						if (a_filter(it->second))
						{
							if (CustomEntryValidateInventoryForm(
									a_params,
									it->second,
									a_config,
									a_hasMinCount))
							{
								return it;
							}
						}
					}
				}
			}

			auto& tmp = a_params.objects.GetTempData().fl;

			tmp.assign(a_config.extraItems.begin(), a_config.extraItems.end());
			tmp.emplace_back(a_config.form.get_id());

			while (tmp.cbegin() != tmp.cend())
			{
				using diff_type = configFormList_t::difference_type;

				RandomNumberGenerator3<diff_type> rng(0, std::distance(tmp.cbegin(), tmp.cend()) - 1);

				const auto ite = tmp.cbegin() + rng.Get(m_rng);

				if (const auto& fid = *ite)
				{
					if (auto it = formData.find(fid); it != formData.end())
					{
						if (a_filter(it->second))
						{
							if (CustomEntryValidateInventoryForm(
									a_params,
									it->second,
									a_config,
									a_hasMinCount))
							{
								return it;
							}
						}
					}
				}

				tmp.erase(ite);
			}
		}
		else
		{
			if (const auto& fid = a_config.form.get_id())
			{
				if (auto it = formData.find(fid); it != formData.end())
				{
					if (a_filter(it->second))
					{
						if (CustomEntryValidateInventoryForm(
								a_params,
								it->second,
								a_config,
								a_hasMinCount))
						{
							return it;
						}
					}
				}
			}

			for (auto& e : a_config.extraItems)
			{
				if (!e)
				{
					continue;
				}

				const auto it = formData.find(e);
				if (it == formData.end())
				{
					continue;
				}

				if (!a_filter(it->second))
				{
					continue;
				}

				if (CustomEntryValidateInventoryForm(
						a_params,
						it->second,
						a_config,
						a_hasMinCount))
				{
					return it;
				}
			}
		}

		return formData.end();
	}

	CollectorData::container_type::const_iterator IEquipment::CustomEntrySelectInventoryForm(
		ProcessParams&      a_params,
		const configCustom_t& a_config,
		ObjectEntryCustom&    a_objectEntry,
		bool&                 a_hasMinCount) noexcept
	{
		if (a_config.customFlags.test(CustomFlags::kLastEquippedMode))
		{
			const auto it = DoLastEquippedSelection(
				a_params,
				a_config.lastEquipped,
				[&](const auto& a_itemEntry) noexcept {
					return CustomEntryValidateInventoryForm(
						a_params,
						a_itemEntry.second,
						a_config,
						a_hasMinCount);
				});

			if (it != a_params.collector.data.forms.end())
			{
				return it;
			}
			else
			{
				return CustomEntrySelectInventoryFormDefault(
					a_params,
					a_config,
					a_objectEntry,
					a_hasMinCount,
					[&](const auto& a_item) noexcept {
						return configBase_t::do_match_sfp(
							a_config.lastEquipped.filterConditions,
							{ a_item.form },
							a_params,
							true);
					});
			}
		}
		else if (a_config.customFlags.test(CustomFlags::kGroupMode))
		{
			return CustomEntrySelectInventoryFormGroup(
				a_params,
				a_config,
				a_objectEntry,
				a_hasMinCount);
		}
		else
		{
			return CustomEntrySelectInventoryFormDefault(
				a_params,
				a_config,
				a_objectEntry,
				a_hasMinCount,
				[](auto&) noexcept { return true; });
		}
	}

	void IEquipment::SelectedItem::consume(
		SlotItemCandidates& a_candidates) const  //
		noexcept(noexcept(SlotItemCandidates().erase(SlotItemCandidates::iterator())))
	{
		assert(item.has_value());

		auto& it = *item;

		if (it->extra == 0)
		{
			a_candidates.erase(it);
		}
		else
		{
			it->extra--;
		}
	}

}