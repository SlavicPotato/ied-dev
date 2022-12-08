#include "pch.h"

#include "FormCommon.h"
#include "Inventory.h"

namespace IED
{
	using namespace Data;

	EntryDataList* GetEntryDataList(Actor* a_actor)
	{
		if (auto containerChanges = a_actor->extraData.Get<ExtraContainerChanges>())
		{
			return containerChanges->data ?
			           containerChanges->data->objList :
			           nullptr;
		}
		else
		{
			return nullptr;
		}
	}

	ItemCandidateCollector::ItemCandidateCollector(
		SlotResults& a_slotResults,
		Actor*       a_actor) :
		data(a_actor),
		slotResults(a_slotResults),
		isPlayer(a_actor == *g_thePlayer)
	{
		if (auto npc = a_actor->GetActorBase())
		{
			Run(*npc, GetEntryDataList(a_actor));
		}
	}

	void ItemCandidateCollector::Run(
		TESContainer&  a_container,
		EntryDataList* a_dataList)
	{
		if (isPlayer)
		{
			data.equippedForms.reserve(20);
		}

		for (auto& e : a_container)
		{
			Process(e);
		}

		if (a_dataList)
		{
			for (auto& e : *a_dataList)
			{
				Process(e);
			}
		}
	}

	bool ItemCandidateCollector::CheckForm(TESForm* a_form)
	{
		if (!a_form)
		{
			return false;
		}

		if (a_form->IsDeleted())  // ?
		{
			return false;
		}

		if (a_form->formID.IsTemporary())  // should be ok to remove this now
		{
			return false;
		}

		return true;
	}

	void ItemCandidateCollector::Process(TESContainer::Entry* a_entry)
	{
		if (!a_entry)
		{
			return;
		}

		const auto form = a_entry->form;

		if (!CheckForm(form))
		{
			return;
		}

		const auto type      = ItemData::GetItemType(form);
		const auto extraType = ItemData::GetItemTypeExtra(form);

		auto& entry = data.forms.try_emplace(
									form->formID,
									form,
									type,
									extraType)
		                  .first->second;

		entry.sharedCount = (entry.count += a_entry->count);

		if (extraType < Data::ObjectTypeExtra::kMax)
		{
			data.typeCount[stl::underlying(extraType)] += a_entry->count;
		}
	}

	void ItemCandidateCollector::Process(InventoryEntryData* a_entryData)
	{
		if (!a_entryData)
		{
			return;
		}

		const auto form = a_entryData->type;

		if (!CheckForm(form))
		{
			return;
		}

		const auto type      = ItemData::GetItemType(form);
		const auto extraType = ItemData::GetItemTypeExtra(form);

		auto& entry = data.forms.try_emplace(
									form->formID,
									form,
									type,
									extraType)
		                  .first->second;

		entry.sharedCount = (entry.count += a_entryData->countDelta);

		if (extraType < Data::ObjectTypeExtra::kMax)
		{
			data.typeCount[stl::underlying(extraType)] += a_entryData->countDelta;
		}

		if (const auto extraLists = a_entryData->GetExtraDataLists())
		{
			for (const auto* e : *extraLists)
			{
				if (!e)
				{
					continue;
				}

				const BSReadLocker locker(e->m_lock);

				const auto* const presence = e->m_presence;
				if (!presence)
				{
					continue;
				}

				if (presence->HasType(ExtraWorn::EXTRA_DATA))
				{
					entry.equipped = true;
				}

				if (presence->HasType(ExtraWornLeft::EXTRA_DATA))
				{
					entry.equippedLeft = true;
				}

				if (presence->HasType(ExtraHotkey::EXTRA_DATA))
				{
					entry.favorited = true;
				}

				if (presence->HasType(ExtraCannotWear::EXTRA_DATA))
				{
					entry.cannotWear = true;
				}
			}
		}

		if (extraType != Data::ObjectTypeExtra::kNone &&
		    (entry.equipped || entry.equippedLeft))
		{
			entry.extraEquipped.type = extraType;

			const auto slot = Data::ItemData::GetSlotFromTypeExtra(extraType);

			if (entry.equipped)
			{
				entry.extraEquipped.slot = slot;

				if (slot < Data::ObjectSlotExtra::kMax)
				{
					data.equippedTypeFlags[stl::underlying(slot)] |= Data::InventoryPresenceFlags::kSet;
				}
			}

			if (entry.equippedLeft)
			{
				const auto slotLeft = Data::ItemData::GetLeftSlotExtra(slot);

				entry.extraEquipped.slotLeft = slotLeft;

				if (slotLeft < Data::ObjectSlotExtra::kMax)
				{
					data.equippedTypeFlags[stl::underlying(slotLeft)] |= Data::InventoryPresenceFlags::kSet;
				}
			}

			data.equippedForms.emplace_back(std::addressof(entry));
		}
	}

	void ItemCandidateCollector::GenerateSlotCandidates(bool a_checkFav)
	{
		for (auto& e : slotResults)
		{
			e.items.clear();
		}

		const bool checkFav = isPlayer && a_checkFav;

		for (const auto& [i, e] : data.forms)
		{
			if (i.IsTemporary())
			{
				continue;
			}

			if (e.type >= ObjectType::kMax)
			{
				continue;
			}

			if (checkFav && !e.favorited)
			{
				continue;
			}

			std::int64_t extra = e.count - 1;

			if (e.equipped)
			{
				extra--;
			}

			if (e.equippedLeft)
			{
				extra--;
			}

			if (extra < 0)
			{
				continue;
			}

			const auto* form = e.form;

			if (isPlayer && !form->GetPlayable())
			{
				continue;
			}

			std::uint32_t rating;

			switch (form->formType)
			{
			case TESObjectWEAP::kTypeID:
				{
					auto weap = static_cast<const TESObjectWEAP*>(form);

					if (weap->IsBound())
					{
						continue;
					}

					rating = weap->attackDamage;
				}
				break;
			case TESAmmo::kTypeID:

				rating = stl::clamped_num_cast<std::uint32_t>(
					static_cast<const TESAmmo*>(form)->settings.damage);

				break;
			case TESObjectARMO::kTypeID:

				rating = static_cast<const TESObjectARMO*>(form)->armorRating;

				break;
			default:

				rating = 0;

				break;
			}

			auto& entry = slotResults[stl::underlying(e.type)];

			entry.items.emplace_back(
				std::addressof(e),
				static_cast<std::uint32_t>(extra),
				rating);
		}

		for (auto& e : slotResults)
		{
			// apparently this is generally faster than inserting into a vector with upper_bound when there's lots of entries

			std::sort(
				e.items.begin(),
				e.items.end(),
				[](auto& a_lhs, auto& a_rhs) [[msvc::forceinline]] {
					return a_lhs.rating > a_rhs.rating;
				});
		}
	}

}