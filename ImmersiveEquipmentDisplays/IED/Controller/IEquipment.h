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

		Data::collectorData_t::container_type::iterator CustomEntrySelectInventoryFormLastEquipped(
			processParams_t&            a_params,
			const Data::configCustom_t& a_config,
			ObjectEntryCustom&          a_objectEntry,
			bool&                       a_hasMinCount);

		Data::collectorData_t::container_type::iterator CustomEntrySelectInventoryFormGroup(
			processParams_t&            a_params,
			const Data::configCustom_t& a_config,
			ObjectEntryCustom&          a_objectEntry,
			bool&                       a_hasMinCount);

		Data::collectorData_t::container_type::iterator CustomEntrySelectInventoryFormDefault(
			processParams_t&                                        a_params,
			const Data::configCustom_t&                             a_config,
			ObjectEntryCustom&                                      a_objectEntry,
			bool&                                                   a_hasMinCount,
			std::function<bool(Data::collectorData_t::itemData_t&)> a_filter = [](auto&) { return true; });

		Data::collectorData_t::container_type::iterator CustomEntrySelectInventoryForm(
			processParams_t&            a_params,
			const Data::configCustom_t& a_config,
			ObjectEntryCustom&          a_objectEntry,
			bool&                       a_hasMinCount);

	private:
		struct
		{
			stl::vector<const BipedSlotEntry*> le;
			Data::configFormList_t             fl;
		} m_temp;

		RandomNumberGeneratorBase& m_rng;
	};

}