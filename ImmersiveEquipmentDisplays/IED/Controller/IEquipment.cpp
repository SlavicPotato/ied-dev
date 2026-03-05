#include "pch.h"

#include "IEquipment.h"

#include "ActorObjectHolder.h"
#include "BipedSlotData.h"
#include "IRNG.h"

#include "IED/Controller/Controller.h"
#include "IED/ProcessParams.h"

namespace IED
{
	using namespace Data;

	IEquipment::IEquipment() noexcept
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
		ProcessParams&            a_params,
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
				    a_config.itemFilterConditions.evaluate_fp(
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

				return a_config.itemFilterConditions.evaluate_fp(
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

	bool IEquipment::ValidateInventoryForm(
		ProcessParams&                       a_params,
		const Data::CollectorData::ItemData& a_itemData,
		const Data::configInventory_t&       a_config) noexcept
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

		if (a_config.flags.test(InventoryFlags::kCheckFav) &&
		    a_params.objects.IsPlayer() &&
		    !a_itemData.is_favorited())
		{
			return false;
		}

		if (a_config.flags.test(InventoryFlags::kEquipmentMode))
		{
			const bool isAmmo = form->IsAmmo();

			if (!isAmmo &&
			    !a_config.flags.test(InventoryFlags::kIgnoreRaceEquipTypes) &&
			    !is_non_shield_armor(form) &&
			    !a_params.objects.IsPlayer())
			{
				if (!a_params.test_equipment_flags(
						ItemData::GetRaceEquipmentFlagFromType(a_itemData.extra.type)))
				{
					return false;
				}
			}

			bool hasMinCount;

			if (isAmmo)
			{
				hasMinCount = !a_itemData.is_equipped() && a_itemData.sharedCount > 0;
			}
			else
			{
				if (a_config.flags.test(InventoryFlags::kDisableIfEquipped) &&
				    a_itemData.is_equipped())
				{
					hasMinCount = false;
				}
				else
				{
					const std::int64_t sharedCount = a_itemData.sharedCount;

					hasMinCount = (sharedCount - a_itemData.get_equip_count()) > 0;
				}
			}

			if (!hasMinCount)
			{
				return false;
			}
		}

		return true;
	}

	CollectorData::container_type::const_iterator IEquipment::ProcessInventoryForm(
		ProcessParams&                 a_params,
		const Data::configInventory_t& a_config,
		const Game::FormID             a_formId,
		const filter_func_t&           a_filter) noexcept
	{
		const auto& formData = a_params.collector.data.forms;

		if (auto it = formData.find(a_formId); it != formData.end())
		{
			if (a_filter(*it))
			{
				if (ValidateInventoryForm(
						a_params,
						it->second,
						a_config))
				{
					return it;
				}
			}
		}

		return formData.end();
	}

	Data::CollectorData::container_type::const_iterator IEquipment::SelectRandomForm(
		ProcessParams&                 a_params,
		const Data::configInventory_t& a_config,
		const Game::FormID             a_primaryFormId,
		const filter_func_t&           a_filter) noexcept
	{
		const auto& formData = a_params.collector.data.forms;

		auto& tmp = a_params.objects.GetTempData().fl;

		tmp.assign(a_config.extraItems.begin(), a_config.extraItems.end());
		if (a_primaryFormId)
		{
			tmp.emplace_back(a_primaryFormId);
		}

		static thread_local std::mt19937 gen([] {
			std::random_device rd;
			return std::mt19937(rd());
		}());

		std::shuffle(tmp.begin(), tmp.end(), gen);

		for (const auto& e : tmp)
		{
			if (const auto it = ProcessInventoryForm(
					a_params,
					a_config,
					e,
					a_filter);
			    it != formData.end())
			{
				return it;
			}
		}

		return formData.end();
	}

	Data::CollectorData::container_type::const_iterator IEquipment::SelectForm(
		ProcessParams&                 a_params,
		const Data::configInventory_t& a_config,
		const Game::FormID             a_primaryFormId,
		const filter_func_t&           a_filter) noexcept
	{
		const auto& formData = a_params.collector.data.forms;

		if (a_primaryFormId)
		{
			if (const auto it = ProcessInventoryForm(
					a_params,
					a_config,
					a_primaryFormId,
					a_filter);
			    it != formData.end())
			{
				return it;
			}
		}

		for (auto& e : a_config.extraItems)
		{
			if (e)
			{
				if (const auto it = ProcessInventoryForm(
						a_params,
						a_config,
						e,
						a_filter);
				    it != formData.end())
				{
					return it;
				}
			}
		}

		return formData.end();
	}

	CollectorData::container_type::const_iterator IEquipment::CustomEntrySelectInventoryFormGroup(
		ProcessParams&              a_params,
		const Data::configCustom_t& a_config) noexcept
	{
		const auto& formData = a_params.collector.data.forms;

		if (const auto& fid = a_config.form.get_id())
		{
			if (auto it = formData.find(fid); it != formData.end())
			{
				if (ValidateInventoryForm(
						a_params,
						it->second,
						a_config.inv))
				{
					return it;
				}
			}
		}

		return formData.end();
	}

	CollectorData::container_type::const_iterator IEquipment::SelectInventoryFormDefault(
		ProcessParams&                 a_params,
		const Data::configInventory_t& a_config,
		const Game::FormID             a_defaultTargetForm,
		const Game::FormID             a_currentForm,
		const filter_func_t&           a_filter) noexcept
	{
		if (a_config.flags.test(InventoryFlags::kSelectInvRandom) &&
		    !a_config.extraItems.empty())
		{
			if (a_currentForm &&
			    (a_currentForm == a_defaultTargetForm ||
			     std::find(
					 a_config.extraItems.begin(),
					 a_config.extraItems.end(),
					 a_currentForm) != a_config.extraItems.end()))
			{
				if (const auto it = ProcessInventoryForm(
						a_params,
						a_config,
						a_currentForm,
						a_filter);
				    it != a_params.collector.data.forms.end())
				{
					return it;
				}
			}

			return SelectRandomForm(
				a_params,
				a_config,
				a_defaultTargetForm,
				a_filter);
		}
		else
		{
			return SelectForm(
				a_params,
				a_config,
				a_defaultTargetForm,
				a_filter);
		}
	}

	CollectorData::container_type::const_iterator IEquipment::CustomEntrySelectInventoryForm(
		ProcessParams&           a_params,
		const configCustom_t&    a_config,
		const ObjectEntryCustom& a_objectEntry) noexcept
	{
		if (a_config.customFlags.test(CustomFlags::kLastEquippedMode))
		{
			const auto it = DoLastEquippedSelection(
				a_params,
				a_config.lastEquipped,
				[&](const auto& a_itemEntry) noexcept {
					return ValidateInventoryForm(
						a_params,
						a_itemEntry.second,
						a_config.inv);
				});

			if (it != a_params.collector.data.forms.end())
			{
				return it;
			}
			else
			{
				return SelectInventoryFormDefault(
					a_params,
					a_config.inv,
					a_config.form.get_id(),
					a_objectEntry.data.state ? a_objectEntry.data.state->form->formID : Game::FormID{},
					[&](const auto& a_item) noexcept {
						return a_config.lastEquipped.filterConditions.evaluate_sfp(
							{ a_item.second.form },
							a_params,
							true);
					});
			}
		}
		else if (a_config.customFlags.test(CustomFlags::kGroupMode))
		{
			return CustomEntrySelectInventoryFormGroup(
				a_params,
				a_config);
		}
		else
		{
			return SelectInventoryFormDefault(
				a_params,
				a_config.inv,
				a_config.form.get_id(),
				a_objectEntry.data.state ? a_objectEntry.data.state->form->formID : Game::FormID{},
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