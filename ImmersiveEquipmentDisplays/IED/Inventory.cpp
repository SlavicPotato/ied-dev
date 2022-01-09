#include "pch.h"

#include "FormCommon.h"
#include "Inventory.h"

#include <skse64/GameData.h>

namespace IED
{
	using namespace Data;

	enum EquipHandSlot
	{
		kLeft,
		kRight,
		kEither,

		kNone
	};

	static EquipHandSlot GetEquipHandSlot(TESForm* item)
	{
		auto equipType = RTTI<BGSEquipType>()(item);
		if (!equipType)
		{
			return EquipHandSlot::kNone;
		}

		auto equipSlot = equipType->GetEquipSlot();
		if (!equipSlot)
		{
			return EquipHandSlot::kNone;
		}

		if (equipSlot == GetEitherHandSlot())
		{
			return EquipHandSlot::kEither;
		}
		else if (equipSlot == GetRightHandSlot())
		{
			return EquipHandSlot::kRight;
		}
		else if (equipSlot == GetLeftHandSlot())
		{
			return EquipHandSlot::kLeft;
		}
		else
		{
			return EquipHandSlot::kNone;
		}
	}

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

	/*bool CanEquipEitherHand(TESObjectWEAP* a_item)
    {
        auto equipSlot = a_item->equipType.GetEquipSlot();
        if (!equipSlot) {
            return false;
        }

        return (
            equipSlot == GetRightHandSlot() ||
            equipSlot == GetEitherHandSlot() ||
            equipSlot == GetLeftHandSlot());
    }*/

	ItemCandidateCollector::ItemCandidateCollector(
		Actor* a_actor,
		TESRace* a_race) :
		m_isPlayer(a_actor == *g_thePlayer),
		m_pm(a_actor->processManager),
		m_data(a_actor, a_race)
	{
		/*m_data.reserve(4000);
		m_equipped.forms.reserve(50);*/
	}

	void ItemCandidateCollector::Run(
		TESContainer& a_container,
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
		if (a_form->IsDeleted())  // ?
		{
			return false;
		}

		if (m_isPlayer && !a_form->IsPlayable())
		{
			return false;
		}

		return true;
	}

	bool ItemCandidateCollector::Accept(TESContainer::ConfigEntry* entry)
	{
		if (!entry)
		{
			return true;
		}

		auto form = entry->form;

		if (!form)
		{
			return true;
		}

		if (!CheckForm(form))
		{
			return true;
		}

		auto type = ItemData::GetItemType(form);

		auto& r = m_data.forms.try_emplace(form->formID, form, type).first->second;

		r.sharedCount = (r.count += entry->count);

		return true;
	}

	SKMP_FORCEINLINE static void AddEquippedForm(
		ActorProcessManager* a_pm,
		TESForm* a_form,
		InventoryEntryData* a_entryData,
		Data::collectorData_t& a_data)
	{
		auto type = Data::ItemData::GetItemTypeExtra(a_form);
		if (type == Data::ObjectTypeExtra::kNone)
		{
			return;
		}

		bool left = false;
		bool right = false;
		bool cannotWear = false;

		if (a_pm)
		{
			if (a_pm->equippedObject[ActorProcessManager::kEquippedHand_Right] == a_form)
			{
				right = true;
			}

			if (a_pm->equippedObject[ActorProcessManager::kEquippedHand_Left] == a_form)
			{
				left = true;
			}
		}

		bool checkLeft = a_form->formType == TESObjectWEAP::kTypeID;

		if (auto extendDataList = a_entryData->extendDataList)
		{
			for (auto it = extendDataList->Begin(); !it.End(); ++it)
			{
				auto extraDataList = *it;
				if (!extraDataList)
				{
					continue;
				}

				if (cannotWear && right && (!checkLeft || left))
				{
					break;
				}

				BSReadLocker locker(std::addressof(extraDataList->m_lock));

				auto presence = extraDataList->m_presence;
				if (!presence)
				{
					continue;
				}

				if (!right)
				{
					right = presence->HasType(ExtraWorn::EXTRA_DATA);
				}

				if (checkLeft && !left)
				{
					left = presence->HasType(ExtraWornLeft::EXTRA_DATA);
				}

				if (!cannotWear)
				{
					cannotWear = presence->HasType(ExtraCannotWear::EXTRA_DATA);
				}
			}
		}

		if (right || left)
		{
			auto r = a_data.forms.try_emplace(a_form->formID, a_form);
			r.first->second.extraEquipped.type = type;

			r.first->second.equipped = right;
			r.first->second.equippedLeft = left;
			r.first->second.cannotWear = cannotWear;

			auto slot = Data::ItemData::GetSlotFromTypeExtra(type);

			if (right)
			{
				r.first->second.extraEquipped.slot = slot;

				auto i = stl::underlying(slot);
				if (i < stl::underlying(Data::ObjectSlotExtra::kMax))
				{
					a_data.slots[i] |= Data::EquippedTypeFlags::kSet;
				}
			}

			if (left)
			{
				r.first->second.extraEquipped.slotLeft = Data::ItemData::GetLeftSlotExtra(slot);

				auto i = stl::underlying(r.first->second.extraEquipped.slotLeft);
				if (i < stl::underlying(Data::ObjectSlotExtra::kMax))
				{
					a_data.slots[i] |= Data::EquippedTypeFlags::kSet;
				}
			}
		}
	}

	/*bool ItemCandidateCollector::AddEquippedForms(
		TESForm* a_form,
		InventoryEntryData* a_entryData)
	{
		switch (a_form->formType)
		{
		case TESObjectARMO::kTypeID:
		case TESObjectWEAP::kTypeID:
		case TESAmmo::kTypeID:
			break;
		case TESObjectLIGH::kTypeID:
			if (!static_cast<TESObjectLIGH*>(a_form)->CanCarry())
			{
				return false;
			}
			break;
		default:
			return true;
		}

		AddEquippedForm(a_form, a_entryData, m_equipped);

		return true;
	}*/

	SKMP_FORCEINLINE static constexpr bool is_equippable(TESForm* a_form) noexcept
	{
		switch (a_form->formType)
		{
		case TESObjectARMO::kTypeID:
		case TESObjectWEAP::kTypeID:
		case TESAmmo::kTypeID:
		case TESObjectLIGH::kTypeID:
			return true;
		default:
			return false;
		}
	}

	/*SKMP_FORCEINLINE bool is_worn(InventoryEntryData* a_entryData) noexcept
	{
		if (auto extendDataList = a_entryData->extendDataList)
		{
			for (auto it = extendDataList->Begin(); !it.End(); ++it)
			{
				auto extraDataList = *it;
				if (!extraDataList)
				{
					continue;
				}

				if (extraDataList->Has<ExtraWorn>())
				{
					return true;
				}
			}
		}

		return false;
	}*/

	bool ItemCandidateCollector::Accept(InventoryEntryData* a_entryData)
	{
		if (!a_entryData)
		{
			return true;
		}

		auto form = a_entryData->type;

		if (!form)
		{
			return true;
		}

		if (!CheckForm(form))
		{
			return true;
		}

		auto type = ItemData::GetItemType(form);

		auto& r = m_data.forms.try_emplace(form->formID, form, type).first->second;

		r.sharedCount = (r.count += a_entryData->countDelta);

		if (!IFormCommon::IsEquippableForm(form))
		{
			return true;
		}

		if (m_pm)
		{
			if (m_pm->equippedObject[ActorProcessManager::kEquippedHand_Right] == form)
			{
				r.equipped = true;
			}

			if (m_pm->equippedObject[ActorProcessManager::kEquippedHand_Left] == form)
			{
				r.equippedLeft = true;
			}
		}

		bool checkLeft = form->formType == TESObjectWEAP::kTypeID;
		bool checkFav = m_isPlayer && type != ObjectType::kMax;

		if (auto extendDataList = a_entryData->extendDataList)
		{
			for (auto it = extendDataList->Begin(); !it.End(); ++it)
			{
				if ((!checkFav || r.favorited) && r.equipped && (!checkLeft || r.equippedLeft))
				{
					break;
				}

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

				if (checkLeft && !r.equippedLeft)
				{
					if (presence->HasType(ExtraWornLeft::EXTRA_DATA))
					{
						r.equippedLeft = true;
					}
				}

				if (checkFav && !r.favorited)
				{
					if (presence->HasType(ExtraHotkey::EXTRA_DATA))
					{
						r.favorited = presence->HasType(ExtraHotkey::EXTRA_DATA);
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

		/*if (form->formType == TESObjectLIGH::kTypeID)
		{
			_DMESSAGE("%X %d %d", form->formID, r.equipped, r.equippedLeft);
		}*/

		if (r.equipped || r.equippedLeft)
		{
			auto type = Data::ItemData::GetItemTypeExtra(form);
			if (type != Data::ObjectTypeExtra::kNone)
			{
				r.extraEquipped.type = type;

				auto slot = Data::ItemData::GetSlotFromTypeExtra(type);

				if (r.equipped)
				{
					r.extraEquipped.slot = slot;

					auto i = stl::underlying(slot);
					if (i < stl::underlying(Data::ObjectSlotExtra::kMax))
					{
						m_data.slots[i] |= Data::EquippedTypeFlags::kSet;
					}
				}

				if (r.equippedLeft)
				{
					r.extraEquipped.slotLeft = Data::ItemData::GetLeftSlotExtra(slot);

					auto i = stl::underlying(r.extraEquipped.slotLeft);
					if (i < stl::underlying(Data::ObjectSlotExtra::kMax))
					{
						m_data.slots[i] |= Data::EquippedTypeFlags::kSet;
					}
				}
			}
		}

		return true;
	}

	void ItemCandidateCollector::GenerateSlotCandidates()
	{
		for (auto& e : m_data.forms)
		{
			if (e.second.type == ObjectType::kMax)
			{
				continue;
			}

			if (m_isPlayer && !e.second.favorited)
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

			if (form->formID.IsTemporary())
			{
				continue;
			}

			std::uint16_t damage;

			if (auto weap = form->As<TESObjectWEAP>())
			{
				if (weap->IsBound())
				{
					continue;
				}

				damage = weap->damage.attackDamage;
			}
			else
			{
				damage = 0;
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

	EquippedFormCollector::EquippedFormCollector(
		Actor* a_actor,
		TESRace* a_race) :
		m_pm(a_actor->processManager),
		m_data(a_actor, a_race)
	{
	}

	bool EquippedFormCollector::Accept(InventoryEntryData* a_entryData)
	{
		if (!a_entryData)
		{
			return true;
		}

		auto form = a_entryData->type;

		if (!form)
		{
			return true;
		}

		if (!IFormCommon::IsEquippableForm(form))
		{
			return true;
		}

		AddEquippedForm(m_pm, form, a_entryData, m_data);

		return true;
	}

	EquippedArmorCollector::EquippedArmorCollector(
		Actor* a_actor,
		TESRace* a_race) :
		m_data(a_actor, a_race)
	{
	}

	void EquippedArmorCollector::Run()
	{
		if (auto list = GetEntryDataList(m_data.actor))
		{
			list->Visit(*this);
		}
	}

	bool EquippedArmorCollector::Accept(InventoryEntryData* a_entryData)
	{
		if (!a_entryData)
		{
			return true;
		}

		auto form = a_entryData->type;

		if (!form)
		{
			return true;
		}

		auto armor = form->As<TESObjectARMO>();

		if (!armor || armor->IsShield())
		{
			return true;
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

				if (extraDataList->Has<ExtraWorn>())
				{
					auto& r = m_data.forms.try_emplace(form->formID, form).first->second;

					//r.equipped = true;

					r.extraEquipped.type = Data::ObjectTypeExtra::kArmor;

					break;
				}
			}
		}

		return true;
	}

}