#pragma once

#include "IED/ConfigStore.h"
#include "IED/Data.h"
#include "IED/Inventory.h"
#include "IED/ProcessParams.h"

#include "IED/Controller/ActorObjectHolder.h"

namespace IED
{
	struct ObjectEntryCustom;
	struct BipedSlotCacheEntry;
	class IRNG;

	class IEquipment
	{
	protected:
		struct SelectedItem
		{
			std::optional<SlotItemCandidates::iterator> item;

			void consume(SlotItemCandidates& a_candidates) const  //
				noexcept(noexcept(SlotItemCandidates().erase(SlotItemCandidates::iterator())));

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
		struct EquippedItemInfo
		{
			TESForm*         right;
			TESForm*         left;
			Data::ObjectSlot rightSlot;
			Data::ObjectSlot leftSlot;
		};

		IEquipment(RandomNumberGeneratorBase& a_rng) noexcept;

		static EquippedItemInfo CreateEquippedItemInfo(
			const ActorProcessManager* const a_pm) noexcept;

		static SelectedItem SelectSlotItem(
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
		static auto DoLastEquippedSelection(
			processParams_t&                  a_params,
			const Data::configLastEquipped_t& a_config,
			Tf                                a_validationFunc) noexcept;

	private:
		static Data::CollectorData::container_type::const_iterator CustomEntrySelectInventoryFormGroup(
			processParams_t&            a_params,
			const Data::configCustom_t& a_config,
			ObjectEntryCustom&          a_objectEntry,
			bool&                       a_hasMinCount) noexcept;

		template <class Tf>
		Data::CollectorData::container_type::const_iterator CustomEntrySelectInventoryFormDefault(
			processParams_t&            a_params,
			const Data::configCustom_t& a_config,
			ObjectEntryCustom&          a_objectEntry,
			bool&                       a_hasMinCount,
			Tf                          a_filter) noexcept;

	public:
		Data::CollectorData::container_type::const_iterator CustomEntrySelectInventoryForm(
			processParams_t&            a_params,
			const Data::configCustom_t& a_config,
			ObjectEntryCustom&          a_objectEntry,
			bool&                       a_hasMinCount) noexcept;

	private:
		template <class Tf>
		static auto SelectInventoryFormLastEquipped(
			processParams_t&                  a_params,
			const Data::configLastEquipped_t& a_config,
			const Tf&                         a_validationFunc) noexcept;

		template <class Tf>
		static auto SelectInventoryFormLastSlotted(
			processParams_t&                  a_params,
			const Data::configLastEquipped_t& a_config,
			const Tf&                         a_validationFunc) noexcept;

		template <class Tf>
		static auto SelectBipedSlotEntryForm(
			processParams_t&                  a_params,
			const Data::configLastEquipped_t& a_config,
			const BipedSlotCacheEntry&        a_slotEntry,
			const Tf&                         a_validationFunc) noexcept;

		template <class Tf>
		static auto SelectDisplaySlotEntryForm(
			processParams_t&                  a_params,
			const Data::configLastEquipped_t& a_config,
			const ObjectEntrySlot&            a_slotEntry,
			const Tf&                         a_validationFunc) noexcept;

		RandomNumberGeneratorBase& m_rng;
	};

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
	auto IEquipment::SelectInventoryFormLastEquipped(
		processParams_t&                  a_params,
		const Data::configLastEquipped_t& a_config,
		const Tf&                         a_validationFunc) noexcept
	{
		const auto& formData = a_params.collector.data.forms;

		const auto& data = a_params.objects.m_slotCache->biped;

		if (a_config.flags.test(Data::LastEquippedFlags::kDisableIfSlotOccupied))
		{
			const auto it = std::find_if(
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

		if (a_config.flags.test(Data::LastEquippedFlags::kPrioritizeRecentBipedSlots) &&
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

				if (a_config.flags.test(Data::LastEquippedFlags::kSkipOccupiedBipedSlots) &&
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
				const auto it = SelectBipedSlotEntryForm(
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

				if (a_config.flags.test(Data::LastEquippedFlags::kSkipOccupiedBipedSlots) &&
				    v.occupied)
				{
					continue;
				}

				const auto it = SelectBipedSlotEntryForm(
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
		const Tf&                         a_validationFunc) noexcept
	{
		const auto& formData = a_params.collector.data.forms;

		if (a_config.flags.test(Data::LastEquippedFlags::kPrioritizeRecentDisplaySlots))
		{
			auto& tmpSlots = a_params.objects.GetTempData().sl;

			tmpSlots.clear();

			for (auto& e : a_config.slots)
			{
				if (e >= Data::ObjectSlot::kMax)
				{
					continue;
				}

				const auto& slot = a_params.objects.GetSlot(e);

				if (a_config.flags.test(Data::LastEquippedFlags::kSkipOccupiedDisplaySlots) &&
				    slot.IsActive())
				{
					continue;
				}

				tmpSlots.emplace_back(std::addressof(slot));
			}

			std::sort(
				tmpSlots.begin(),
				tmpSlots.end(),
				[](auto& a_lhs, auto& a_rhs) [[msvc::forceinline]] {
					return a_lhs->slotState.lastOccupied >
				           a_rhs->slotState.lastOccupied;
				});

			for (auto& e : tmpSlots)
			{
				const auto it = SelectDisplaySlotEntryForm(
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
			for (auto& e : a_config.slots)
			{
				if (e >= Data::ObjectSlot::kMax)
				{
					continue;
				}

				const auto& slot = a_params.objects.GetSlot(e);

				if (a_config.flags.test(Data::LastEquippedFlags::kSkipOccupiedDisplaySlots) &&
				    slot.IsActive())
				{
					continue;
				}

				const auto it = SelectDisplaySlotEntryForm(
					a_params,
					a_config,
					slot,
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
	auto IEquipment::SelectBipedSlotEntryForm(
		processParams_t&                  a_params,
		const Data::configLastEquipped_t& a_config,
		const BipedSlotCacheEntry&        a_slotEntry,
		const Tf&                         a_validationFunc) noexcept
	{
		const auto& formData = a_params.collector.data.forms;

		for (auto& formid : a_slotEntry.forms)
		{
			const auto it = formData.find(formid);
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

	template <class Tf>
	auto IEquipment::SelectDisplaySlotEntryForm(
		processParams_t&                  a_params,
		const Data::configLastEquipped_t& a_config,
		const ObjectEntrySlot&            a_slotEntry,
		const Tf&                         a_validationFunc) noexcept
	{
		const auto& formData = a_params.collector.data.forms;

		for (const auto& form : a_slotEntry.slotState.lastSlotted)
		{
			const auto it = formData.find(form);
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