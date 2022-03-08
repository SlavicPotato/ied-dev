#include "pch.h"

#include "IEquipment.h"

namespace IED
{
	using namespace Data;

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

	auto IEquipment::SelectItem(
		Actor*                            a_actor,
		const Data::configSlot_t&         a_entry,
		SlotItemCandidates::storage_type& a_candidates,
		Game::FormID                      a_lastEquipped)
		-> selectedItem_t
	{
		if (a_candidates.empty())
		{
			return {};
		}

		bool checkCannotWear = a_entry.slotFlags.test(Data::SlotFlags::kCheckCannotWear);

		if (!a_entry.preferredItems.empty())
		{
			for (auto& e : a_entry.preferredItems)
			{
				auto it = std::find_if(
					a_candidates.begin(),
					a_candidates.end(),
					[id = e](const auto& a_item) {
						return (id == a_item.form->formID);
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
			if (a_entry.itemFilter.test(a_lastEquipped))
			{
				auto it = std::find_if(
					a_candidates.begin(),
					a_candidates.end(),
					[&](const auto& a_item) {
						return (a_item.form->formID == a_lastEquipped);
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

				return a_entry.itemFilter.test(a_item.form->formID);
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

	void IEquipment::selectedItem_t::consume(
		SlotItemCandidates::storage_type& a_candidates)
	{
		auto it(*item);

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