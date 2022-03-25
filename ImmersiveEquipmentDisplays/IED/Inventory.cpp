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
		m_isPlayer(a_actor == *g_thePlayer),
		m_data(a_actor)
	{
	}

	void ItemCandidateCollector::Run(
		TESContainer&                          a_container,
		RE::BSSimpleList<InventoryEntryData*>* a_dataList)
	{
		for (auto& e : a_container)
		{
			Accept(e);
		}

		if (a_dataList)
		{
			for (auto& e : *a_dataList)
			{
				Accept(e);
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

	bool ItemCandidateCollector::Accept(TESContainer::Entry* entry)
	{
		if (!entry)
		{
			return true;
		}

		auto form = entry->form;

		if (!CheckForm(form))
		{
			return true;
		}

		auto extraType = ItemData::GetItemTypeExtra(form);

		auto& r = m_data.forms.try_emplace(
								  form->formID,
								  form,
								  ItemData::GetItemType(form),
								  extraType)
		              .first->second;

		r.sharedCount = (r.count += entry->count);

		if (auto i = stl::underlying(extraType);
		    i < stl::underlying(Data::ObjectTypeExtra::kMax))
		{
			m_data.typeCount[i] += entry->count;
		}

		return true;
	}

	bool ItemCandidateCollector::Accept(InventoryEntryData* a_entryData)
	{
		if (!a_entryData)
		{
			return true;
		}

		auto form = a_entryData->type;

		if (!CheckForm(form))
		{
			return true;
		}

		auto type      = ItemData::GetItemType(form);
		auto extraType = ItemData::GetItemTypeExtra(form);

		auto& r = m_data.forms.try_emplace(
								  form->formID,
								  form,
								  type,
								  extraType)
		              .first->second;

		r.sharedCount = (r.count += a_entryData->countDelta);

		if (auto i = stl::underlying(extraType);
		    i < stl::underlying(Data::ObjectTypeExtra::kMax))
		{
			m_data.typeCount[i] += a_entryData->countDelta;
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

				auto& presence = e->m_presence;
				if (!presence)
				{
					continue;
				}

				if (!r.equipped)
				{
					if (presence->HasType(ExtraWorn::EXTRA_DATA))
					{
						r.equipped = true;
					}
				}

				if (!r.equippedLeft)
				{
					if (presence->HasType(ExtraWornLeft::EXTRA_DATA))
					{
						r.equippedLeft = true;
					}
				}

				if (m_isPlayer && !r.favorited)
				{
					if (presence->HasType(ExtraHotkey::EXTRA_DATA))
					{
						r.favorited = true;
					}
				}

				if (!r.cannotWear)
				{
					if (presence->HasType(ExtraCannotWear::EXTRA_DATA))
					{
						r.cannotWear = true;
					}
				}
			}
		}

		if (extraType != Data::ObjectTypeExtra::kNone &&
		    (r.equipped || r.equippedLeft))
		{
			r.extraEquipped.type = extraType;

			auto slot = Data::ItemData::GetSlotFromTypeExtra(extraType);

			if (r.equipped)
			{
				r.extraEquipped.slot = slot;

				auto i = stl::underlying(slot);
				if (i < stl::underlying(Data::ObjectSlotExtra::kMax))
				{
					m_data.equippedTypeFlags[i] |= Data::InventoryPresenceFlags::kSet;
				}
			}

			if (r.equippedLeft)
			{
				r.extraEquipped.slotLeft = Data::ItemData::GetLeftSlotExtra(slot);

				auto i = stl::underlying(r.extraEquipped.slotLeft);
				if (i < stl::underlying(Data::ObjectSlotExtra::kMax))
				{
					m_data.equippedTypeFlags[i] |= Data::InventoryPresenceFlags::kSet;
				}
			}

			m_data.equippedForms.emplace_back(std::addressof(r));
		}

		return true;
	}

	void ItemCandidateCollector::GenerateSlotCandidates(bool a_checkFav)
	{
		bool checkFav = m_isPlayer && a_checkFav;

		for (const auto& e : m_data.forms)
		{
			if (e.second.type == ObjectType::kMax)
			{
				continue;
			}

			if (checkFav && !e.second.favorited)
			{
				continue;
			}

			std::int64_t extra = e.second.count - 1;

			if (e.second.equipped)
			{
				extra--;
			}

			if (e.second.equippedLeft)
			{
				extra--;
			}

			if (extra < 0)
			{
				continue;
			}

			auto form = e.second.form;

			if (m_isPlayer && !form->IsPlayable())
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

					rating = weap->damage.attackDamage;
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

			auto& entry = m_slotResults[stl::underlying(e.second.type)];

			entry.m_items.emplace_back(form, rating, extra, std::addressof(e.second));
		}

		for (auto& e : m_slotResults)
		{
			// apparently this is generally faster than inserting into a vector with upper_bound when there's lots of entries

			std::sort(
				e.m_items.begin(),
				e.m_items.end(),
				[](auto& a_lhs, auto& a_rhs) {
					return a_lhs.rating > a_rhs.rating;
				});
		}
	}

}