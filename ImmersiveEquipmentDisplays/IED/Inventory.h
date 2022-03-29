#pragma once

#include "CollectorData.h"
#include "Controller/ControllerCommon.h"
#include "Data.h"

namespace IED
{
	class Controller;

	RE::BSSimpleList<InventoryEntryData*>* GetEntryDataList(Actor* a_actor);

	struct SlotItemCandidates
	{
		struct item_t
		{
			const Data::collectorData_t::itemData_t* item;
			std::uint32_t                            extra;
			std::uint32_t                            rating;
		};

		using storage_type = std::vector<item_t>;

		storage_type m_items;
	};

	struct ItemCandidateCollector
	{
	public:
		ItemCandidateCollector(
			Actor* a_actor);

		void Run(
			TESContainer&                          a_container,
			RE::BSSimpleList<InventoryEntryData*>* a_dataList);

		SKMP_FORCEINLINE void Process(TESContainer::Entry* entry);
		SKMP_FORCEINLINE void Process(InventoryEntryData* a_entryData);

		void GenerateSlotCandidates(bool a_checkFav);

		[[nodiscard]] inline constexpr auto& GetCandidates(Data::ObjectType a_type) noexcept
		{
			return m_slotResults[stl::underlying(a_type)].m_items;
		}

		Data::collectorData_t m_data;
		SlotItemCandidates    m_slotResults[stl::underlying(Data::ObjectType::kMax)];

	private:
		SKMP_FORCEINLINE bool CheckForm(TESForm* a_form);

		bool m_isPlayer;
	};

}