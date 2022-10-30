#include "pch.h"

#include "ActorInventoryInfo.h"

#include "IED/LocaleData.h"

#include <skse64/HashUtil.h>

namespace IED
{
	void actorInventoryInfo_t::Update(
		TESContainer&                          a_container,
		RE::BSSimpleList<InventoryEntryData*>* a_dataList)
	{
		items.clear();

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

		items.sortvec([](auto& a_lhs, auto& a_rhs) [[msvc::forceinline]] {
			return a_lhs->first < a_rhs->first;
		});
	}

	auto actorInventoryInfo_t::AddOrGetBaseItem(TESBoundObject* a_item)
		-> Base&
	{
		auto r = items.try_emplace(
			a_item->formID,
			a_item->formID,
			a_item->formType);

		auto& e = r.first->second;

		if (r.second)
		{
			if (auto fullname = ::RTTI<TESFullName>::Cast(a_item))
			{
				if (auto name = fullname->GetFullName())
				{
					e.name = LocaleData::ToUTF8(name);
				}
			}

			if (auto enchantable = ::RTTI<TESEnchantableForm>()(a_item))
			{
				if (auto enchant = enchantable->formEnchanting)
				{
					e.enchantment.emplace(enchant->formID);
					if (auto name = enchant->GetFullName())
					{
						e.enchantment->name = LocaleData::ToUTF8(name);
					}
				}
			}

			if (!a_item->GetPlayable())
			{
				e.flags.set(InventoryInfoBaseFlags::kNonPlayable);
			}
		}

		return e;
	}

	void actorInventoryInfo_t::Process(TESContainer::Entry* a_entry)
	{
		if (!a_entry)
		{
			return;
		}

		auto form = a_entry->form;

		if (!form)
		{
			return;
		}

		auto& item = AddOrGetBaseItem(form);

		item.count += a_entry->count;
		item.baseCount = a_entry->count;
	}

	void actorInventoryInfo_t::Process(InventoryEntryData* a_entryData)
	{
		if (!a_entryData)
		{
			return;
		}

		auto form = a_entryData->type;

		if (!form)
		{
			return;
		}

		auto& item = AddOrGetBaseItem(form);

		item.count += a_entryData->countDelta;
		item.deltaCount = a_entryData->countDelta;

		if (const auto extraLists = a_entryData->GetExtraDataLists())
		{
			for (auto* e : *extraLists)
			{
				if (!e)
				{
					continue;
				}

				auto& x = item.extraList.emplace_back();

				BSReadLocker locker(e->m_lock);

				if (const auto extraEnchant = e->GetImpl<ExtraEnchantment>())
				{
					if (auto enchant = extraEnchant->enchant)
					{
						x.enchantment.emplace(enchant->formID);
						if (auto name = enchant->GetFullName())
						{
							x.enchantment->name = LocaleData::ToUTF8(name);
						}
					}
				}

				float healthValue = 1.0f;

				if (const auto extraHealth = e->GetImpl<ExtraHealth>())
				{
					x.health = healthValue = extraHealth->health;
					x.flags.set(InventoryInfoExtraFlags::kHasHealth);
				}

				if (auto xText = e->GetExtraTextDisplayData_Internal())
				{
					if (auto name = xText->GenerateName(form, healthValue))
					{
						x.name = LocaleData::ToUTF8(name);
					}
				}

				if (const auto extraUID = e->GetImpl<ExtraUniqueID>())
				{
					x.uid = extraUID->uniqueId;
				}

				//x.itemId = HashUtil::CRC32(x.GetName(item).c_str(), form->formID & 0x00FFFFFFui32);

				if (const auto extraOwner = e->GetImpl<ExtraOwnership>())
				{
					if (auto owner = extraOwner->owner)
					{
						x.owner = owner->formID;
					}
				}

				if (const auto extraOriginalRefr = e->GetImpl<ExtraOriginalReference>())
				{
					NiPointer<TESObjectREFR> ref;
					if (extraOriginalRefr->handle.Lookup(ref))
					{
						x.originalRefr = ref->formID;
					}
				}

				const auto* presence = e->m_presence;
				if (!presence)
				{
					continue;
				}

				if (presence->HasType(ExtraWorn::EXTRA_DATA))
				{
					x.flags.set(InventoryInfoExtraFlags::kEquipped);
					item.flags.set(InventoryInfoBaseFlags::kEquipped);
				}

				if (presence->HasType(ExtraWornLeft::EXTRA_DATA))
				{
					x.flags.set(InventoryInfoExtraFlags::kEquippedLeft);
					item.flags.set(InventoryInfoBaseFlags::kEquippedLeft);
				}

				if (presence->HasType(ExtraHotkey::EXTRA_DATA))
				{
					x.flags.set(InventoryInfoExtraFlags::kFavorite);
				}

				if (presence->HasType(ExtraCannotWear::EXTRA_DATA))
				{
					x.flags.set(InventoryInfoExtraFlags::kCannotWear);
				}

				if (presence->HasType(ExtraPoison::EXTRA_DATA))
				{
					x.flags.set(InventoryInfoExtraFlags::kHasPoison);
				}

				if (presence->HasType(ExtraShouldWear::EXTRA_DATA))
				{
					x.flags.set(InventoryInfoExtraFlags::kShouldWear);
				}
			}
		}
	}

}