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
			if (auto containerData = containerChanges->data)
			{
				return containerData->objList;
			}
		}

		return nullptr;
	}

	ItemCandidateCollector::ItemCandidateCollector(
		Actor* a_actor) :
		m_isPlayer(a_actor == *g_thePlayer),
		m_data(a_actor)
	{
	}

	void ItemCandidateCollector::Run(
		TESContainer&  a_container,
		EntryDataList* a_dataList)
	{
		a_container.Visit(*this);

		if (a_dataList)
		{
			a_dataList->Visit(*this);
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

		if (auto extendDataList = a_entryData->extendDataList)
		{
			for (auto it = extendDataList->Begin(); !it.End(); ++it)
			{
				auto extraDataList = *it;
				if (!extraDataList)
				{
					continue;
				}

				BSReadLocker locker(std::addressof(extraDataList->m_lock));

				auto presence = extraDataList->m_presence;
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
		}

		return true;
	}

	void ItemCandidateCollector::GenerateSlotCandidates(bool a_checkFav)
	{
		bool checkFav = m_isPlayer && a_checkFav;

		for (auto& e : m_data.forms)
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

			std::uint16_t damage;

			switch (form->formType)
			{
			case TESObjectWEAP::kTypeID:
				{
					auto weap = static_cast<TESObjectWEAP*>(form);

					if (weap->IsBound())
					{
						continue;
					}

					damage = weap->damage.attackDamage;
				}
				break;
			case TESAmmo::kTypeID:

				damage = static_cast<std::uint16_t>(
					std::clamp(
						static_cast<TESAmmo*>(form)->settings.damage,
						static_cast<float>(
							std::numeric_limits<std::uint16_t>::min()),
						static_cast<float>(
							std::numeric_limits<std::uint16_t>::max())));

				break;
			default:
				damage = 0;
				break;
			}

			auto& entry = m_slotResults[stl::underlying(e.second.type)];

			entry.m_items.emplace_back(form, damage, extra, std::addressof(e.second));
		}

		for (auto& e : m_slotResults)
		{
			// apparently this is generally faster than inserting into a vector with upper_bound when there's lots of entries

			std::sort(
				e.m_items.begin(),
				e.m_items.end(),
				[](auto& a_lhs, auto& a_rhs) {
					return a_lhs.damage > a_rhs.damage;
				});
		}
	}

}