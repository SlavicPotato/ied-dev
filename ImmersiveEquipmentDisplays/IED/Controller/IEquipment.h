#pragma once

#include "IED/ConfigStore.h"
#include "IED/Data.h"
#include "IED/Inventory.h"
#include "IED/ProcessParams.h"

#include "IED/Controller/ActorObjectHolder.h"

namespace IED
{
	struct equippedItemInfo_t
	{
		TESForm*         right;
		TESForm*         left;
		Data::ObjectSlot rightSlot;
		Data::ObjectSlot leftSlot;
	};

	struct ObjectEntryCustom;
	struct BipedSlotCacheEntry;
	class IRNG;

	class IEquipment
	{
	protected:
		struct selectedItem_t
		{
			std::optional<SlotItemCandidates::iterator> item;

			void consume(SlotItemCandidates& a_candidates) const  //
				noexcept(std::is_nothrow_move_assignable_v<SlotItemCandidates::value_type>);

			[[nodiscard]] constexpr auto* operator->() const noexcept
			{
				return std::addressof(*(*item));
			}

			[[nodiscard]] constexpr explicit operator bool() const noexcept
			{
				return item.has_value();
			}
		};

	public:
		IEquipment(RandomNumberGeneratorBase& a_rng) noexcept;

		static equippedItemInfo_t CreateEquippedItemInfo(
			const ActorProcessManager* const a_pm) noexcept;

		static selectedItem_t SelectSlotItem(
			processParams_t&          a_params,
			const Data::configSlot_t& a_config,
			SlotItemCandidates&       a_candidates,
			const ObjectEntrySlot&    a_slot) noexcept;

		static bool CustomEntryValidateInventoryForm(
			processParams_t&                     a_params,
			const Data::CollectorData::ItemData& a_itemData,
			const Data::configCustom_t&          a_config,
			bool&                                a_hasMinCount) noexcept;

		template <class Tf>
		auto SelectInventoryFormLastEquipped(
			processParams_t&                  a_params,
			const Data::configLastEquipped_t& a_config,
			Tf                                a_validationFunc) noexcept;

		template <class Tf>
		static auto SelectInventoryFormLastSlotted(
			processParams_t&                  a_params,
			const Data::configLastEquipped_t& a_config,
			Tf                                a_validationFunc) noexcept;

		template <class Tf>
		auto DoLastEquippedSelection(
			processParams_t&                  a_params,
			const Data::configLastEquipped_t& a_config,
			Tf                                a_validationFunc) noexcept;

		static Data::CollectorData::container_type::iterator CustomEntrySelectInventoryFormGroup(
			processParams_t&            a_params,
			const Data::configCustom_t& a_config,
			ObjectEntryCustom&          a_objectEntry,
			bool&                       a_hasMinCount) noexcept;

		template <class Tf>
		Data::CollectorData::container_type::iterator CustomEntrySelectInventoryFormDefault(
			processParams_t&            a_params,
			const Data::configCustom_t& a_config,
			ObjectEntryCustom&          a_objectEntry,
			bool&                       a_hasMinCount,
			Tf                          a_filter) noexcept;

		Data::CollectorData::container_type::iterator CustomEntrySelectInventoryForm(
			processParams_t&            a_params,
			const Data::configCustom_t& a_config,
			ObjectEntryCustom&          a_objectEntry,
			bool&                       a_hasMinCount) noexcept;

	private:
		template <class Tf>
		static constexpr auto SelectSlotEntryForm(
			processParams_t&                  a_params,
			const Data::configLastEquipped_t& a_config,
			const BipedSlotCacheEntry&        a_slotEntry,
			Tf                                a_validationFunc) noexcept;

		RandomNumberGeneratorBase& m_rng;
	};

	template <class Tf>
	auto IEquipment::SelectInventoryFormLastEquipped(
		processParams_t&                  a_params,
		const Data::configLastEquipped_t& a_config,
		Tf                                a_validationFunc) noexcept
	{
		auto& formData = a_params.collector.data.forms;

		const auto& data = a_params.objects.m_slotCache->biped;

		if (a_config.flags.test(Data::LastEquippedFlags::kDisableIfSlotOccupied))
		{
			auto it = std::find_if(
				a_config.bipedSlots.begin(),
				a_config.bipedSlots.end(),
				[&](auto& a_v) noexcept [[msvc::forceinline]] {
					const auto slot = a_params.translate_biped_object(a_v);

					return slot < BIPED_OBJECT::kTotal &&
				           data[stl::underlying(slot)].occupied;
				});

			if (it != a_config.bipedSlots.end())
			{
				return formData.end();
			}
		}

		if (a_config.flags.test(Data::LastEquippedFlags::kPrioritizeRecentSlots) &&
		    a_config.bipedSlots.size() > 1)
		{
			auto& bipedSlots = a_params.objects.GetTempData().le;

			bipedSlots.clear();

			for (auto& e : a_config.bipedSlots)
			{
				const auto slot = a_params.translate_biped_object(e);

				if (slot >= BIPED_OBJECT::kTotal)
				{
					continue;
				}

				auto& v = data[stl::underlying(slot)];

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
				auto it = SelectSlotEntryForm(
					a_params,
					a_config,
					*e,
					a_validationFunc);

				if (it != formData.end())
				{
					return it;
				}
			}
		}
		else
		{
			for (auto& e : a_config.bipedSlots)
			{
				const auto slot = a_params.translate_biped_object(e);

				if (slot >= BIPED_OBJECT::kTotal)
				{
					continue;
				}

				auto& v = data[stl::underlying(slot)];

				if (a_config.flags.test(Data::LastEquippedFlags::kSkipOccupiedSlots) &&
				    v.occupied)
				{
					continue;
				}

				auto it = SelectSlotEntryForm(
					a_params,
					a_config,
					v,
					a_validationFunc);

				if (it != formData.end())
				{
					return it;
				}
			}
		}

		return formData.end();
	}

	template <class Tf>
	auto IEquipment::SelectInventoryFormLastSlotted(
		processParams_t&                  a_params,
		const Data::configLastEquipped_t& a_config,
		Tf                                a_validationFunc) noexcept
	{
		auto& formData = a_params.collector.data.forms;

		for (auto& e : a_config.slots)
		{
			if (e < Data::ObjectSlot::kMax)
			{
				const auto& slot = a_params.objects.GetSlot(e);

				for (const auto& form : slot.slotState.lastSlotted)
				{
					auto it = formData.find(form);
					if (it != formData.end())
					{
						if (Data::configBase_t::do_match_sfp(
								a_config.filterConditions,
								{ it->second.form },
								a_params,
								true))
						{
							if (a_validationFunc(*it))
							{
								return it;
							}
						}
					}
				}
			}
		}

		return formData.end();
	}

	template <class Tf>
	auto IEquipment::DoLastEquippedSelection(
		processParams_t&                  a_params,
		const Data::configLastEquipped_t& a_config,
		Tf                                a_validationFunc) noexcept
	{
		auto it = SelectInventoryFormLastEquipped(
			a_params,
			a_config,
			a_validationFunc);

		if (it == a_params.collector.data.forms.end() &&
		    a_config.flags.test(Data::LastEquippedFlags::kFallBackToSlotted))
		{
			it = SelectInventoryFormLastSlotted(
				a_params,
				a_config,
				a_validationFunc);
		}

		return it;
	}

	template <class Tf>
	constexpr auto IEquipment::SelectSlotEntryForm(
		processParams_t&                  a_params,
		const Data::configLastEquipped_t& a_config,
		const BipedSlotCacheEntry&        a_slotEntry,
		Tf                                a_validationFunc) noexcept
	{
		auto& formData = a_params.collector.data.forms;

		for (auto& formid : a_slotEntry.forms)
		{
			auto it = formData.find(formid);
			if (it == formData.end())
			{
				continue;
			}

			if (!Data::configBase_t::do_match_sfp(
					a_config.filterConditions,
					{ it->second.form },
					a_params,
					true))
			{
				continue;
			}

			if (a_validationFunc(*it))
			{
				return it;
			}
		}

		return formData.end();
	}

}