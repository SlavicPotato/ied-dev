#include "pch.h"

#include "FormCommon.h"
#include "Inventory.h"

namespace IED
{
	using namespace Data;

	EntryDataList* GetEntryDataList(const Actor* const a_actor) noexcept
	{
		if (const auto* const containerChanges = a_actor->extraData.Get<ExtraContainerChanges>())
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

	InventoryInfoCollector::InventoryInfoCollector(
		SlotResults&                            a_slotResults,
		Data::CollectorData::container_type&    a_idt,
		Data::CollectorData::eq_container_type& a_eqt,
		const Actor* const                      a_actor) noexcept :
		data(a_idt, a_eqt),
		slotResults(a_slotResults),
		isPlayer(a_actor == *g_thePlayer)
	{
		if (const auto* const npc = a_actor->GetActorBase())
		{
			Run(*npc, GetEntryDataList(a_actor));
		}
	}

	void InventoryInfoCollector::Run(
		const TESContainer&  a_container,
		EntryDataList* const a_dataList) noexcept
	{
		if (a_dataList)
		{
			ProcessList(a_dataList);
		}

		ProcessList(a_container);
		PostProcess();
	}

	void InventoryInfoCollector::ProcessList(
		const TESContainer& a_container) noexcept
	{
		for (const auto* const e : a_container)
		{
			if (e)
			{
				Process(e);
			}
		}
	}

	void InventoryInfoCollector::ProcessList(
		EntryDataList* const a_dataList) noexcept
	{
		for (const auto* const e : *a_dataList)
		{
			if (e)
			{
				Process(e);
			}
		}

		data.forms.sort_data();
	}

	void InventoryInfoCollector::PostProcess() noexcept
	{
		for (auto& e : data.forms)
		{
			const auto typeExtra = e.second.extra.typeExtra;

			if (typeExtra < Data::ObjectTypeExtra::kMax &&
			    e.second.itemCount > 0 &&
			    !data.IsTypePresent(typeExtra))
			{
				data.SetTypePresent(typeExtra);
			}
		}
	}

	SKMP_FORCEINLINE static constexpr bool check_form(const TESForm* a_form) noexcept
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

	void InventoryInfoCollector::Process(
		const TESContainer::Entry* a_entry) noexcept
	{
		const auto form = a_entry->form;

		if (!check_form(form))
		{
			return;
		}

		auto& entry = data.forms.try_emplace(
									form->formID,
									form,
									[f = form]() [[msvc::forceinline]] {
										return ItemData::GetItemTypePair(f);
									})
		                  .first->second;

		entry.sharedCount = (entry.itemCount += a_entry->count);
	}

	void InventoryInfoCollector::Process(
		const InventoryEntryData* a_entryData) noexcept
	{
		const auto form = a_entryData->type;

		if (!check_form(form))
		{
			return;
		}

		const auto typePair   = ItemData::GetItemTypePair(form);
		const auto countDelta = a_entryData->countDelta;

		auto& entry = data.forms.raw()
		                  .emplace_back(
							  std::piecewise_construct,
							  std::forward_as_tuple(form->formID),
							  std::forward_as_tuple(
								  form,
								  typePair,
								  countDelta))
		                  .second;

		if (const auto extraLists = a_entryData->GetExtraDataLists())
		{
			for (const auto* const e : *extraLists)
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
					entry.extra.flags.set(CollectorData::ItemFlags::kEquipped);
				}

				if (presence->HasType(ExtraWornLeft::EXTRA_DATA))
				{
					entry.extra.flags.set(CollectorData::ItemFlags::kEquippedLeft);
				}

				if (presence->HasType(ExtraHotkey::EXTRA_DATA))
				{
					entry.extra.flags.set(CollectorData::ItemFlags::kFavorite);
				}

				if (presence->HasType(ExtraCannotWear::EXTRA_DATA))
				{
					entry.extra.flags.set(CollectorData::ItemFlags::kCannotWear);
				}
			}
		}

		if (entry.is_equipped())
		{
			if (typePair.typeExtra < Data::ObjectTypeExtra::kMax)
			{
				const auto slot = ItemData::GetSlotFromTypeExtra(typePair.typeExtra);

				if (slot < Data::ObjectSlotExtra::kMax)
				{
					if (entry.is_equipped_right())
					{
						entry.extra.equipped.slot = slot;

						data.SetSlotEquipped(slot);
					}

					if (entry.is_equipped_left())
					{
						const auto slotLeft = ItemData::GetLeftSlotExtra(slot);

						if (slotLeft < Data::ObjectSlotExtra::kMax)
						{
							entry.extra.equipped.slotLeft = slotLeft;

							data.SetSlotEquipped(slotLeft);
						}
					}
				}
			}

			data.equippedForms.emplace_back(entry);
		}
	}

	void InventoryInfoCollector::GenerateSlotCandidates(bool a_checkFav) noexcept
	{
		for (auto& e : slotResults)
		{
			e.clear();
		}

		const bool checkFav = isPlayer && a_checkFav;

		for (const auto& [i, e] : data.forms)
		{
			if (e.extra.type >= ObjectType::kMax)
			{
				continue;
			}

			/*if (i.IsTemporary())
			{
				continue;
			}*/

			if (checkFav && !e.is_favorited())
			{
				continue;
			}

			const auto extra = (static_cast<std::int64_t>(e.itemCount) - e.get_equip_count()) - 1;

			if (extra < 0)
			{
				continue;
			}

			const auto* const form = e.form;

			std::uint32_t rating;

			switch (form->formType)
			{
			case TESObjectWEAP::kTypeID:
				{
					const auto weap = static_cast<const TESObjectWEAP*>(form);

					if (weap->IsBound())
					{
						continue;
					}

					if (isPlayer && weap->weaponData.flags.test(TESObjectWEAP::Data::Flag::kNonPlayable))
					{
						continue;
					}

					rating = weap->attackDamage;
				}
				break;
			case TESAmmo::kTypeID:
				{
					const auto ammo = static_cast<const TESAmmo*>(form);

					if (isPlayer && ammo->settings.flags.test(AMMO_DATA::Flag::kNonPlayable))
					{
						continue;
					}

					rating = stl::clamped_num_cast<std::uint32_t>(ammo->settings.damage);
				}
				break;
			case TESObjectARMO::kTypeID:
				{
					const auto armor = static_cast<const TESObjectARMO*>(form);

					if (isPlayer && !armor->_IsPlayable())
					{
						continue;
					}

					rating = armor->armorRating;
				}
				break;
			case TESObjectLIGH::kTypeID:
				{
					const auto light = static_cast<const TESObjectLIGH*>(form);

					if (isPlayer && !light->_IsPlayable())
					{
						continue;
					}

					rating = 0;
				}
				break;
			default:
				continue;
			}

			auto& entry = slotResults[stl::underlying(e.extra.type)];

			entry.emplace_back(
				std::addressof(e),
				static_cast<std::uint32_t>(extra),
				rating);
		}

		for (auto& e : slotResults)
		{
			std::sort(
				e.begin(),
				e.end(),
				[](auto& a_lhs, auto& a_rhs) [[msvc::forceinline]] {
					return a_lhs.rating > a_rhs.rating;
				});
		}
	}

}