#include "pch.h"

#include "FormCommon.h"
#include "Inventory.h"

namespace IED
{
	using namespace Data;

	RE::BSSimpleList<InventoryEntryData*>* GetEntryDataList(Actor* a_actor)
	{
		if (auto containerChanges = a_actor->extraData.Get<ExtraContainerChanges>())
		{
			return containerChanges->data ?
			           containerChanges->data->GetObjList() :
                       nullptr;
		}
		else
		{
			return nullptr;
		}
	}

	ItemCandidateCollector::ItemCandidateCollector(
		Actor* a_actor) :
		isPlayer(a_actor == *g_thePlayer),
		data(a_actor)
	{
	}

	void ItemCandidateCollector::Run(
		TESContainer&                          a_container,
		RE::BSSimpleList<InventoryEntryData*>* a_dataList)
	{
		if (isPlayer)
		{
			data.forms.reserve(1000);
			data.equippedForms.reserve(20);
		}
		else
		{
			data.forms.reserve(100);
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

		if (a_form->formID.IsTemporary())  // ?
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

		auto form = a_entry->form;

		if (!CheckForm(form))
		{
			return;
		}

		auto type      = ItemData::GetItemType(form);
		auto extraType = ItemData::GetItemTypeExtra(form);

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

		if (type < Data::ObjectType::kMax)
		{
			slotResults[stl::underlying(type)].reserve++;
		}

		return;
	}

	void ItemCandidateCollector::Process(InventoryEntryData* a_entryData)
	{
		if (!a_entryData)
		{
			return;
		}

		auto form = a_entryData->type;

		if (!CheckForm(form))
		{
			return;
		}

		auto type      = ItemData::GetItemType(form);
		auto extraType = ItemData::GetItemTypeExtra(form);

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

		if (type < Data::ObjectType::kMax)
		{
			slotResults[stl::underlying(type)].reserve++;
		}

		if (auto extendDataList = a_entryData->GetDataList())
		{
			for (auto& e : *extendDataList)
			{
				if (!e)
				{
					continue;
				}

				BSReadLocker locker(e->m_lock);

				auto presence = e->m_presence;
				if (!presence)
				{
					continue;
				}

				if (!entry.equipped)
				{
					if (presence->HasType(ExtraWorn::EXTRA_DATA))
					{
						entry.equipped = true;
					}
				}

				if (!entry.equippedLeft)
				{
					if (presence->HasType(ExtraWornLeft::EXTRA_DATA))
					{
						entry.equippedLeft = true;
					}
				}

				if (isPlayer && !entry.favorited)
				{
					if (presence->HasType(ExtraHotkey::EXTRA_DATA))
					{
						entry.favorited = true;
					}
				}

				if (!entry.cannotWear)
				{
					if (presence->HasType(ExtraCannotWear::EXTRA_DATA))
					{
						entry.cannotWear = true;
					}
				}
			}
		}

		if (extraType != Data::ObjectTypeExtra::kNone &&
		    (entry.equipped || entry.equippedLeft))
		{
			entry.extraEquipped.type = extraType;

			auto slot = Data::ItemData::GetSlotFromTypeExtra(extraType);

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
				auto slotLeft = Data::ItemData::GetLeftSlotExtra(slot);

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
			e.items.reserve(e.reserve);
		}

		bool checkFav = isPlayer && a_checkFav;

		for (const auto& [i, e] : data.forms)
		{
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

			auto form = e.form;

			if (isPlayer && !form->GetPlayable())
			{
				continue;
			}

			std::uint32_t rating;

			switch (form->formType)
			{
			case TESObjectWEAP::kTypeID:
				{
					auto weap = static_cast<TESObjectWEAP*>(form);

					if (weap->IsBound())
					{
						continue;
					}

					rating = weap->attackDamage;
				}
				break;
			case TESAmmo::kTypeID:

				rating = stl::clamped_num_cast<std::uint32_t>(
					static_cast<TESAmmo*>(form)->settings.damage);

				break;
			case TESObjectARMO::kTypeID:

				rating = static_cast<TESObjectARMO*>(form)->armorRating;

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
				[](auto& a_lhs, auto& a_rhs) {
					return a_lhs.rating > a_rhs.rating;
				});
		}
	}

}