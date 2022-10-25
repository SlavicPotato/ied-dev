#include "pch.h"

#include "IEquipment.h"

#include "ActorObjectHolder.h"
#include "BipedSlotData.h"
#include "IRNG.h"

#include "IED/ProcessParams.h"

namespace IED
{
	using namespace Data;

	IEquipment::IEquipment(RandomNumberGeneratorBase& a_rng) :
		m_rng(a_rng)
	{
	}

	auto IEquipment::CreateEquippedItemInfo(ActorProcessManager* a_pm)
		-> equippedItemInfo_t
	{
		auto formLeft  = a_pm->equippedObject[ActorProcessManager::kEquippedHand_Left];
		auto formRight = a_pm->equippedObject[ActorProcessManager::kEquippedHand_Right];

		return equippedItemInfo_t{
			formLeft,
			formRight,
			formLeft ?
				ItemData::GetObjectSlotLeft(formLeft) :
                ObjectSlot::kMax,
			formRight ?
				ItemData::GetObjectSlot(formRight) :
                ObjectSlot::kMax
		};
	}

	auto IEquipment::SelectSlotItem(
		const Data::configSlot_t&         a_config,
		SlotItemCandidates::storage_type& a_candidates,
		Game::FormID                      a_lastEquipped)
		-> selectedItem_t
	{
		if (a_candidates.empty())
		{
			return {};
		}

		bool checkCannotWear = a_config.slotFlags.test(Data::SlotFlags::kCheckCannotWear);

		if (!a_config.preferredItems.empty())
		{
			for (auto& e : a_config.preferredItems)
			{
				auto it = std::find_if(
					a_candidates.begin(),
					a_candidates.end(),
					[&](const auto& a_item) {
						return (e == a_item.item->form->formID);
					});

				if (it != a_candidates.end())
				{
					if (!checkCannotWear ||
					    !it->item->cannotWear)
					{
						return { it };
					}
				}
			}
		}

		if (a_lastEquipped)
		{
			if (a_config.itemFilter.test(a_lastEquipped))
			{
				auto it = std::find_if(
					a_candidates.begin(),
					a_candidates.end(),
					[&](const auto& a_item) {
						return (a_item.item->form->formID == a_lastEquipped);
					});

				if (it != a_candidates.end())
				{
					if (!checkCannotWear ||
					    !it->item->cannotWear)
					{
						return { it };
					}
				}
			}
		}

		auto it = std::find_if(
			a_candidates.begin(),
			a_candidates.end(),
			[&](const auto& a_item) {
				if (checkCannotWear && a_item.item->cannotWear)
				{
					return false;
				}

				return a_config.itemFilter.test(a_item.item->form->formID);
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

	inline static constexpr bool is_non_shield_armor(TESForm* a_form) noexcept
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
		processParams_t&                   a_params,
		const collectorData_t::itemData_t& a_itemData,
		const configCustom_t&              a_config,
		bool&                              a_hasMinCount)
	{
		if (a_itemData.count < 1)
		{
			return false;
		}

		if ((a_config.countRange.min && a_itemData.count < a_config.countRange.min) ||
		    a_config.countRange.max && a_itemData.count > a_config.countRange.max)
		{
			return false;
		}

		if (a_config.customFlags.test(CustomFlags::kCheckFav) &&
		    a_params.objects.IsPlayer() &&
		    !a_itemData.favorited)
		{
			return false;
		}

		if (a_config.customFlags.test_any(CustomFlags::kEquipmentModeMask))
		{
			bool isAmmo = a_itemData.form->IsAmmo();

			if (!isAmmo &&
			    !a_config.customFlags.test(CustomFlags::kIgnoreRaceEquipTypes) &&
			    !is_non_shield_armor(a_itemData.form) &&
			    !a_params.objects.IsPlayer())
			{
				if (!a_params.test_equipment_flags(
						ItemData::GetRaceEquipmentFlagFromType(a_itemData.type)))
				{
					return false;
				}
			}

			if (isAmmo)
			{
				a_hasMinCount = !a_itemData.is_equipped();
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
					std::uint32_t delta = 0;

					if (a_itemData.equipped)
					{
						delta++;
					}

					if (a_itemData.equippedLeft)
					{
						delta++;
					}

					a_hasMinCount = a_itemData.sharedCount - delta > 0;
				}
			}
		}
		else
		{
			a_hasMinCount = true;
		}

		return true;
	}

	collectorData_t::container_type::iterator IEquipment::CustomEntrySelectInventoryFormGroup(
		processParams_t&            a_params,
		const Data::configCustom_t& a_config,
		ObjectEntryCustom&          a_objectEntry,
		bool&                       a_hasMinCount)
	{
		auto& formData = a_params.collector.data.forms;

		if (a_config.form.get_id() &&
		    !a_config.form.get_id().IsTemporary())
		{
			if (auto it = formData.find(a_config.form.get_id()); it != formData.end())
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
	collectorData_t::container_type::iterator IEquipment::CustomEntrySelectInventoryFormDefault(
		processParams_t&            a_params,
		const Data::configCustom_t& a_config,
		ObjectEntryCustom&          a_objectEntry,
		bool&                       a_hasMinCount,
		Tf                          a_filter)
	{
		auto& formData = a_params.collector.data.forms;

		if (a_config.customFlags.test(CustomFlags::kSelectInvRandom) &&
		    !a_config.extraItems.empty())
		{
			if (a_objectEntry.data.state)
			{
				auto fid = a_objectEntry.data.state->formid;

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

			auto& tmp = m_temp.fl;

			tmp.assign(a_config.extraItems.begin(), a_config.extraItems.end());
			tmp.emplace_back(a_config.form.get_id());

			while (tmp.begin() != tmp.end())
			{
				using diff_type = configFormList_t::difference_type;

				RandomNumberGenerator3<diff_type> rng(0, std::distance(tmp.begin(), tmp.end()) - 1);

				auto ite = tmp.begin() + rng.Get(m_rng);

				if (*ite && !ite->IsTemporary())
				{
					if (auto it = formData.find(*ite); it != formData.end())
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
			if (a_config.form.get_id() &&
			    !a_config.form.get_id().IsTemporary())
			{
				if (auto it = formData.find(a_config.form.get_id()); it != formData.end())
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
				if (e && !e.IsTemporary())
				{
					if (auto it = formData.find(e); it != formData.end())
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
		}

		return formData.end();
	}

	collectorData_t::container_type::iterator IEquipment::CustomEntrySelectInventoryForm(
		processParams_t&      a_params,
		const configCustom_t& a_config,
		ObjectEntryCustom&    a_objectEntry,
		bool&                 a_hasMinCount)
	{
		if (a_config.customFlags.test(CustomFlags::kLastEquippedMode))
		{
			auto it = DoLastEquippedSelection(
				a_params,
				a_config.lastEquipped,
				[&](auto& a_itemData) {
					return CustomEntryValidateInventoryForm(
						a_params,
						a_itemData,
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
					[&](auto& a_item) {
						return configBase_t::do_match_fp(
							a_config.lastEquipped.filterConditions,
							{ a_item.form, ItemData::GetItemSlotExtraGeneric(a_item.form) },
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
				[](auto&) { return true; });
		}
	}

	void IEquipment::selectedItem_t::consume(
		SlotItemCandidates::storage_type& a_candidates) const
	{
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