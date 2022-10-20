#pragma once

#include "IED/ConfigStore.h"
#include "IED/Data.h"
#include "IED/Inventory.h"

namespace IED
{
	struct equippedItemInfo_t
	{
		TESForm*         left;
		TESForm*         right;
		Data::ObjectSlot leftSlot;
		Data::ObjectSlot rightSlot;
	};

	struct ObjectEntryCustom;
	struct processParams_t;
	struct BipedSlotEntry;
	class IRNG;

	class IEquipment
	{
	protected:
		struct selectedItem_t
		{
			stl::optional<SlotItemCandidates::storage_type::iterator> item;

			void consume(SlotItemCandidates::storage_type& a_candidates) const;

			[[nodiscard]] inline constexpr auto* operator->() const noexcept
			{
				return std::addressof(*(*item));
			}

			[[nodiscard]] inline constexpr explicit operator bool() const noexcept
			{
				return item.has();
			}
		};

	public:
		IEquipment(RandomNumberGeneratorBase& a_rng);

		static equippedItemInfo_t CreateEquippedItemInfo(ActorProcessManager* a_pm);

		static selectedItem_t SelectSlotItem(
			const Data::configSlot_t&         a_config,
			SlotItemCandidates::storage_type& a_candidates,
			Game::FormID                      a_lastEquipped);

		bool CustomEntryValidateInventoryForm(
			processParams_t&                         a_params,
			const Data::collectorData_t::itemData_t& a_itemData,
			const Data::configCustom_t&              a_config,
			bool&                                    a_hasMinCount);

		template <class Tf>
		Data::collectorData_t::container_type::iterator SelectInventoryFormLastEquipped(
			processParams_t&                  a_params,
			const Data::configLastEquipped_t& a_config,
			Tf                                a_validationFunc);

		Data::collectorData_t::container_type::iterator CustomEntrySelectInventoryFormGroup(
			processParams_t&            a_params,
			const Data::configCustom_t& a_config,
			ObjectEntryCustom&          a_objectEntry,
			bool&                       a_hasMinCount);

		template <class Tf>
		Data::collectorData_t::container_type::iterator CustomEntrySelectInventoryFormDefault(
			processParams_t&            a_params,
			const Data::configCustom_t& a_config,
			ObjectEntryCustom&          a_objectEntry,
			bool&                       a_hasMinCount,
			Tf                          a_filter);

		Data::collectorData_t::container_type::iterator CustomEntrySelectInventoryForm(
			processParams_t&            a_params,
			const Data::configCustom_t& a_config,
			ObjectEntryCustom&          a_objectEntry,
			bool&                       a_hasMinCount);

	private:
		template <class Tf>
		SKMP_FORCEINLINE std::optional<Data::collectorData_t::container_type::iterator> SelectSlotEntryForm(
			processParams_t&                  a_params,
			const Data::configLastEquipped_t& a_config,
			const BipedSlotEntry&             a_slotEntry,
			Tf                                a_validationFunc);

		struct
		{
			stl::vector<const BipedSlotEntry*> le;
			Data::configFormList_t             fl;
		} m_temp;

		RandomNumberGeneratorBase& m_rng;
	};

	template <class Tf>
	Data::collectorData_t::container_type::iterator IEquipment::SelectInventoryFormLastEquipped(
		processParams_t&                  a_params,
		const Data::configLastEquipped_t& a_config,
		Tf                                a_validationFunc)
	{
		auto& formData = a_params.collector.data.forms;

		const auto& data = a_params.objects.m_lastEquipped->data;

		if (a_config.flags.test(Data::LastEquippedFlags::kDisableIfSlotOccupied))
		{
			auto it = std::find_if(
				a_config.bipedSlots.begin(),
				a_config.bipedSlots.end(),
				[&](auto& a_v) [[msvc::forceinline]] {
					return a_v < BIPED_OBJECT::kTotal &&
				           data[stl::underlying(a_v)].occupied;
				});

			if (it != a_config.bipedSlots.end())
			{
				return formData.end();
			}
		}

		if (a_config.flags.test(Data::LastEquippedFlags::kPrioritizeRecentSlots) &&
		    a_config.bipedSlots.size() > 1)
		{
			auto& bipedSlots = m_temp.le;

			bipedSlots.clear();
			bipedSlots.reserve(a_config.bipedSlots.size());

			for (auto& e : a_config.bipedSlots)
			{
				if (e >= BIPED_OBJECT::kTotal)
				{
					continue;
				}

				auto& v = data[stl::underlying(e)];

				if (a_config.flags.test(Data::LastEquippedFlags::kSkipOccupiedSlots) &&
				    v.occupied)
				{
					continue;
				}

				bipedSlots.emplace_back(std::addressof(v));
			}

			std::sort(
				bipedSlots.begin(),
				bipedSlots.end(),
				[](auto& a_lhs, auto& a_rhs) [[msvc::forceinline]] {
					return a_lhs->seen > a_rhs->seen;
				});

			for (auto& e : bipedSlots)
			{
				if (auto r = SelectSlotEntryForm(
						a_params,
						a_config,
						*e,
						a_validationFunc))
				{
					return *r;
				}
			}
		}
		else
		{
			for (auto& e : a_config.bipedSlots)
			{
				if (e >= BIPED_OBJECT::kTotal)
				{
					continue;
				}

				auto& v = data[stl::underlying(e)];

				if (a_config.flags.test(Data::LastEquippedFlags::kSkipOccupiedSlots) &&
				    v.occupied)
				{
					continue;
				}

				if (auto r = SelectSlotEntryForm(
						a_params,
						a_config,
						v,
						a_validationFunc))
				{
					return *r;
				}
			}
		}

		return formData.end();
	}

	template <class Tf>
	std::optional<Data::collectorData_t::container_type::iterator> IEquipment::SelectSlotEntryForm(
		processParams_t&                  a_params,
		const Data::configLastEquipped_t& a_config,
		const BipedSlotEntry&             a_slotEntry,
		Tf                                a_validationFunc)
	{
		auto& formData = a_params.collector.data.forms;

		for (auto& formid : a_slotEntry.forms)
		{
			if (!formid || formid.IsTemporary())
			{
				continue;
			}

			auto it = formData.find(formid);
			if (it == formData.end())
			{
				continue;
			}

			if (!Data::configBase_t::do_match_fp(
					a_config.filterConditions,
					{ it->second.form, ItemData::GetItemSlotExtraGeneric(it->second.form) },
					a_params,
					true))
			{
				continue;
			}

			if (a_validationFunc(it->second))
			{
				return it;
			}
		}

		return {};
	}

}