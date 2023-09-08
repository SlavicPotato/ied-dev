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
			const Actor* a_actor) noexcept;

		static SelectedItem SelectSlotItem(
			ProcessParams&            a_params,
			const Data::configSlot_t& a_config,
			SlotItemCandidates&       a_candidates,
			const ObjectEntrySlot&    a_slot) noexcept;

		static bool CustomEntryValidateInventoryForm(
			ProcessParams&                       a_params,
			const Data::CollectorData::ItemData& a_itemData,
			const Data::configCustom_t&          a_config,
			bool&                                a_hasMinCount) noexcept;

		template <class Tf>
		static auto DoLastEquippedSelection(
			ProcessParams&                    a_params,
			const Data::configLastEquipped_t& a_config,
			Tf                                a_validationFunc) noexcept;

	private:
		static Data::CollectorData::container_type::const_iterator CustomEntrySelectInventoryFormGroup(
			ProcessParams&              a_params,
			const Data::configCustom_t& a_config,
			ObjectEntryCustom&          a_objectEntry,
			bool&                       a_hasMinCount) noexcept;

		template <class Tf>
		Data::CollectorData::container_type::const_iterator CustomEntrySelectInventoryFormDefault(
			ProcessParams&              a_params,
			const Data::configCustom_t& a_config,
			ObjectEntryCustom&          a_objectEntry,
			bool&                       a_hasMinCount,
			Tf                          a_filter) noexcept;

	public:
		Data::CollectorData::container_type::const_iterator CustomEntrySelectInventoryForm(
			ProcessParams&              a_params,
			const Data::configCustom_t& a_config,
			ObjectEntryCustom&          a_objectEntry,
			bool&                       a_hasMinCount) noexcept;

	private:
		template <class Tf>
		static auto SelectInventoryFormLastEquipped(
			ProcessParams&                    a_params,
			const Data::configLastEquipped_t& a_config,
			const Tf&                         a_validationFunc) noexcept;

		template <class Tf>
		static auto SelectInventoryFormLastSlotted(
			ProcessParams&                    a_params,
			const Data::configLastEquipped_t& a_config,
			const Tf&                         a_validationFunc) noexcept;

		template <class Tf>
		static auto SelectInventoryFormLastAcquired(
			ProcessParams&                    a_params,
			const Data::configLastEquipped_t& a_config,
			const Tf&                         a_validationFunc) noexcept;

		template <class Tf>
		static auto SelectEntryForm(
			ProcessParams&                                          a_params,
			const Data::configLastEquipped_t&                       a_config,
			const stl::boost_vector<Data::configFormZeroMissing_t>& a_forms,
			const Tf&                                               a_validationFunc) noexcept;

		RandomNumberGeneratorBase& m_rng;
	};

	template <class Tf>
	auto IEquipment::DoLastEquippedSelection(
		ProcessParams&                    a_params,
		const Data::configLastEquipped_t& a_config,
		Tf                                a_validationFunc) noexcept
	{
		auto it = SelectInventoryFormLastEquipped(
			a_params,
			a_config,
			a_validationFunc);

		if (it != a_params.collector.data.forms.end())
		{
			return it;
		}

		if (a_config.flags.test(Data::LastEquippedFlags::kFallBackToSlotted))
		{
			it = SelectInventoryFormLastSlotted(
				a_params,
				a_config,
				a_validationFunc);

			if (it != a_params.collector.data.forms.end())
			{
				return it;
			}
		}

		if (a_config.flags.test(Data::LastEquippedFlags::kFallBackToAcquired))
		{
			it = SelectInventoryFormLastAcquired(
				a_params,
				a_config,
				a_validationFunc);
		}

		return it;
	}

	template <class Tf>
	auto IEquipment::SelectInventoryFormLastEquipped(
		ProcessParams&                    a_params,
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
			auto& bipedSlots = a_params.objects.GetTempData().pc;

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
					return reinterpret_cast<const BipedSlotCacheEntry*>(a_lhs)->seen >
				           reinterpret_cast<const BipedSlotCacheEntry*>(a_rhs)->seen;
				});

			for (auto& e : bipedSlots)
			{
				const auto it = SelectEntryForm(
					a_params,
					a_config,
					reinterpret_cast<const BipedSlotCacheEntry*>(e)->forms,
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

				const auto it = SelectEntryForm(
					a_params,
					a_config,
					v.forms,
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
		ProcessParams&                    a_params,
		const Data::configLastEquipped_t& a_config,
		const Tf&                         a_validationFunc) noexcept
	{
		const auto& formData = a_params.collector.data.forms;

		if (a_config.flags.test(Data::LastEquippedFlags::kPrioritizeRecentDisplaySlots))
		{
			auto& tmpSlots = a_params.objects.GetTempData().pc;

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
					return reinterpret_cast<const ObjectEntrySlot*>(a_lhs)->slotState.lastOccupied >
				           reinterpret_cast<const ObjectEntrySlot*>(a_rhs)->slotState.lastOccupied;
				});

			for (auto& e : tmpSlots)
			{
				const auto it = SelectEntryForm(
					a_params,
					a_config,
					reinterpret_cast<const ObjectEntrySlot*>(e)->slotState.lastSlotted,
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

				const auto it = SelectEntryForm(
					a_params,
					a_config,
					slot.slotState.lastSlotted,
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
	auto IEquipment::SelectInventoryFormLastAcquired(
		ProcessParams&                    a_params,
		const Data::configLastEquipped_t& a_config,
		const Tf&                         a_validationFunc) noexcept
	{
		const auto& formData = a_params.collector.data.forms;
		const auto& bsd      = a_params.objects.GetBipedSlotData();

		if (a_config.flags.test(Data::LastEquippedFlags::kPrioritizeRecentAcquiredTypes))
		{
			auto& tmpSlots = a_params.objects.GetTempData().pc;

			tmpSlots.clear();

			for (auto& e : a_config.acqList)
			{
				if (e >= Data::ObjectTypeExtra::kMax)
				{
					continue;
				}

				tmpSlots.emplace_back(std::addressof(bsd->get(e)));
			}

			std::sort(
				tmpSlots.begin(),
				tmpSlots.end(),
				[](auto& a_lhs, auto& a_rhs) [[msvc::forceinline]] {
					return reinterpret_cast<const AcquiredCacheEntry*>(a_lhs)->lastAcquired >
				           reinterpret_cast<const AcquiredCacheEntry*>(a_rhs)->lastAcquired;
				});

			for (auto& e : tmpSlots)
			{
				const auto it = SelectEntryForm(
					a_params,
					a_config,
					reinterpret_cast<const AcquiredCacheEntry*>(e)->forms,
					a_validationFunc);

				if (it != formData.end())
				{
					return it;
				}
			}
		}
		else
		{
			for (auto& e : a_config.acqList)
			{
				if (e >= Data::ObjectTypeExtra::kMax)
				{
					continue;
				}

				const auto& entry = bsd->get(e);

				const auto it = SelectEntryForm(
					a_params,
					a_config,
					entry.forms,
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
	auto IEquipment::SelectEntryForm(
		ProcessParams&                                          a_params,
		const Data::configLastEquipped_t&                       a_config,
		const stl::boost_vector<Data::configFormZeroMissing_t>& a_forms,
		const Tf&                                               a_validationFunc) noexcept
	{
		const auto& formData = a_params.collector.data.forms;

		for (auto& formid : a_forms)
		{
			const auto it = formData.find(formid);
			if (it == formData.end())
			{
				continue;
			}

			if (!a_config.filterConditions.evaluate_sfp(
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